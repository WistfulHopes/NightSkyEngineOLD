#include "FighterGameState.h"
#include "EngineUtils.h"
#include "FighterAIController.h"
#include "FighterEngine/Miscellaneous/BattleUIActor.h"
#include "FighterEngine/Miscellaneous/FighterGameInstance.h"
#include "FighterPlayerController.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraActor.h"
#include "Components/AudioComponent.h"
#include "FighterRunners/FighterLocalRunner.h"
#include "FighterRunners/FighterMultiplayerRunner.h"
#include "FighterRunners/FighterSynctestRunner.h"
#include "Kismet/GameplayStatics.h"

AFighterGameState::AFighterGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AFighterGameState::BeginPlay()
{
	Super::BeginPlay();
	LocalFrame = INITIAL_FRAME;
	RemoteFrame = INITIAL_FRAME;
	SyncFrame = INITIAL_FRAME;
	RemoteFrameAdvantage = 0;
	Init();
}

void AFighterGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (const UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport(); !ViewportClient->Viewport->IsForegroundWindow())
	{
		if(AFighterPlayerController* PlayerController = Cast<AFighterPlayerController>(GetWorld()->GetFirstPlayerController()))
			PlayerController->FlushPressedKeys();
	}
	
	FighterRunner->Update(DeltaSeconds);

	UpdateCamera();
	UpdateUI();

	if (GetNetMode() != NM_Standalone)
	{
		FGGPONetworkStats Network = GetNetworkStats();
		NetworkStats.Ping = Network.network.ping;
		int32 LocalFramesBehind = Network.timesync.local_frames_behind;
		int32 RemoteFramesBehind = Network.timesync.remote_frames_behind;

		if (LocalFramesBehind < 0 && RemoteFramesBehind < 0)
			NetworkStats.RollbackFrames = abs(abs(LocalFramesBehind) - abs(RemoteFramesBehind));
		else if (LocalFramesBehind > 0 && RemoteFramesBehind > 0)
			NetworkStats.RollbackFrames = 0;
		else
			NetworkStats.RollbackFrames = abs(LocalFramesBehind) + abs(RemoteFramesBehind);
	}
}

void AFighterGameState::TickGameState()
{
	{
		RemoteFrame++;
		LocalFrame++;
		UpdateLocalInput();
		Update(LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0], LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1]);
	}
}

int AFighterGameState::GetLocalInputs(int Index)
{
	if (const AFighterPlayerController* Controller = Cast<AFighterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), Index)); Controller != nullptr)
	{
		return Controller->Inputs;
	}
	if (AFighterAIController* Controller = Cast<AFighterAIController>(UAIBlueprintHelperLibrary::GetAIController(Players[3])); Controller != nullptr)
	{
		return Controller->Inputs;
	}
	return 0;
}

void AFighterGameState::UpdateLocalInput()
{
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0] = GetLocalInputs(0);
		LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1] = GetLocalInputs(1);
		return;
	}
	const int PlayerIndex = Cast<UFighterGameInstance>(GetGameInstance())->PlayerIndex;
	int SendInputs[MAX_ROLLBACK_FRAMES] = { 16 };
	if (PlayerIndex == 0)
	{
		RemoteInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0] = LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0] = GetLocalInputs(0);
		for (int i = 0; i < MAX_ROLLBACK_FRAMES; i++)
		{
			SendInputs[i] = LocalInputs[i][0];
		}
		Cast<AFighterPlayerController>(GetWorld()->GetFirstPlayerController())->UpdateInput(SendInputs, LocalFrame, LocalFrameAdvantage);
		RemoteInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1] = LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1] = LocalInputs[RemoteFrame % MAX_ROLLBACK_FRAMES][1];
	}
	else
	{
		RemoteInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0] = LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0] = LocalInputs[RemoteFrame % MAX_ROLLBACK_FRAMES][0];
		RemoteInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1] = LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1] = GetLocalInputs(0);
		for (int i = 0; i < MAX_ROLLBACK_FRAMES; i++)
		{
			SendInputs[i] = LocalInputs[i][1];
		}
		Cast<AFighterPlayerController>(GetWorld()->GetFirstPlayerController())->UpdateInput(SendInputs, LocalFrame, LocalFrameAdvantage);
	}
}

void AFighterGameState::UpdateRemoteInput(int RemoteInput[], int32 InFrame, int32 InFrameAdvantage)
{
	RemoteFrame = InFrame;
	RemoteFrameAdvantage = InFrameAdvantage;
	const int PlayerIndex = Cast<UFighterGameInstance>(GetGameInstance())->PlayerIndex;
	if (PlayerIndex == 0)
	{
		for (int i = InFrame; i > InFrame - MAX_ROLLBACK_FRAMES; i--)
		{
			RemoteInputs[i % MAX_ROLLBACK_FRAMES][1] = RemoteInput[i % MAX_ROLLBACK_FRAMES];
		}
	}
	else
	{
		for (int i = InFrame; i > InFrame - MAX_ROLLBACK_FRAMES; i--)
		{
			RemoteInputs[i % MAX_ROLLBACK_FRAMES][0] = RemoteInput[i % MAX_ROLLBACK_FRAMES];
		}
	}
}

void AFighterGameState::HandleRoundWin()
{
	if (BattleState.RoundFormat < ERoundFormat::TwoVsTwo)
	{
		if (Players[0]->CurrentHealth > 0 && Players[3]->CurrentHealth <= 0)
		{
			if (!Players[0]->RoundWinInputLock)
				BattleState.P1RoundsWon++;
			Players[0]->RoundWinTimer--;
			Players[0]->RoundWinInputLock = true;
			BattleState.PauseTimer = true;
			if (Players[0]->RoundWinTimer <= 0)
			{
				BattleState.PauseTimer = false;
				HandleMatchWin();
				RoundInit();
			}
		}
		else if (Players[3]->CurrentHealth > 0 && Players[0]->CurrentHealth <= 0)
		{
			if (!Players[3]->RoundWinInputLock)
				BattleState.P2RoundsWon++;
			Players[3]->RoundWinTimer--;
			Players[3]->RoundWinInputLock = true;
			BattleState.PauseTimer = true;
			if (Players[3]->RoundWinTimer <= 0)
			{
				BattleState.PauseTimer = false;
				HandleMatchWin();
				RoundInit();
			}
		}
		else if (Players[0]->CurrentHealth <= 0 && Players[3]->CurrentHealth <= 0)
		{
			if (!Players[0]->RoundWinInputLock)
			{
				BattleState.P1RoundsWon++;
				BattleState.P2RoundsWon++;
			}
			Players[0]->RoundWinInputLock = true;
			Players[0]->RoundWinTimer--;
			BattleState.PauseTimer = true;
			if (Players[0]->RoundWinTimer <= 0)
			{
				BattleState.PauseTimer = false;
				HandleMatchWin();
				RoundInit();
			}
		}
		else if (BattleState.RoundTimer <= 0)
		{
			if (Players[0]->CurrentHealth > Players[3]->CurrentHealth)
			{
				if (!Players[0]->RoundWinInputLock)
					BattleState.P1RoundsWon++;
				Players[0]->RoundWinTimer--;
				Players[0]->RoundWinInputLock = true;
				BattleState.PauseTimer = true;
				if (Players[0]->RoundWinTimer <= 0)
				{
					BattleState.PauseTimer = false;
					HandleMatchWin();
					RoundInit();
				}
			}
			else if (Players[3]->CurrentHealth > Players[0]->CurrentHealth)
			{
				if (!Players[3]->RoundWinInputLock)
					BattleState.P2RoundsWon++;
				Players[3]->RoundWinTimer--;
				Players[3]->RoundWinInputLock = true;
				BattleState.PauseTimer = true;
				if (Players[3]->RoundWinTimer <= 0)
				{
					BattleState.PauseTimer = false;
					HandleMatchWin();
					RoundInit();
				}
			}
			else if (Players[0]->CurrentHealth == Players[3]->CurrentHealth)
			{
				if (!Players[0]->RoundWinInputLock)
				{
					BattleState.P1RoundsWon++;
					BattleState.P2RoundsWon++;
				}
				Players[0]->RoundWinInputLock = true;
				Players[0]->RoundWinTimer--;
				BattleState.PauseTimer = true;
				if (Players[0]->RoundWinTimer <= 0)
				{
					BattleState.PauseTimer = false;
					HandleMatchWin();
					RoundInit();
				}
			}
		}
	}
}

void AFighterGameState::HandleMatchWin()
{
	switch (BattleState.RoundFormat)
	{
	case ERoundFormat::FirstToOne:
		if (BattleState.P1RoundsWon > 0 && BattleState.P2RoundsWon < BattleState.P1RoundsWon)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (BattleState.P2RoundsWon > 0 && BattleState.P1RoundsWon < BattleState.P2RoundsWon)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (BattleState.P1RoundsWon == 2 && BattleState.P2RoundsWon == 2)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		return;
	case ERoundFormat::FirstToTwo:
		if (BattleState.P1RoundsWon > 1 && BattleState.P2RoundsWon < BattleState.P1RoundsWon)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (BattleState.P2RoundsWon > 1 && BattleState.P1RoundsWon < BattleState.P2RoundsWon)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (BattleState.P1RoundsWon == 3 && BattleState.P2RoundsWon == 3)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		return;
	case ERoundFormat::FirstToThree:
		if (BattleState.P1RoundsWon > 2 && BattleState.P2RoundsWon < BattleState.P1RoundsWon)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (BattleState.P2RoundsWon > 2 && BattleState.P1RoundsWon < BattleState.P2RoundsWon)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (BattleState.P1RoundsWon == 4 && BattleState.P2RoundsWon == 4)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		return;
	case ERoundFormat::FirstToFour:
		if (BattleState.P1RoundsWon > 3 && BattleState.P2RoundsWon < BattleState.P1RoundsWon)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (BattleState.P2RoundsWon > 3 && BattleState.P1RoundsWon < BattleState.P2RoundsWon)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (BattleState.P1RoundsWon == 5 && BattleState.P2RoundsWon == 5)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		return;
	case ERoundFormat::FirstToFive:
		if (BattleState.P1RoundsWon > 4 && BattleState.P2RoundsWon < BattleState.P1RoundsWon)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (BattleState.P2RoundsWon > 4 && BattleState.P1RoundsWon < BattleState.P2RoundsWon)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		else if (BattleState.P1RoundsWon == 6 && BattleState.P2RoundsWon == 6)
		{
			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		}
		return;
	default:
		return;
	}
}

void AFighterGameState::RoundInit()
{
	BattleState.UniversalGauge[0] = 10000;
	BattleState.UniversalGauge[1] = 10000;
	for (int i = 0; i < 400; i++)
		Objects[i]->ResetObject();
	
	for (int i = 0; i < 6; i++)
		Players[i]->ResetForRound();

	Players[0]->IsOnScreen = true;
	Players[3]->IsOnScreen = true;
	
	UFighterGameInstance* GameInstance = Cast<UFighterGameInstance>(GetGameInstance());

	BattleState.RoundTimer = GameInstance->StartRoundTimer * 60;
	BattleState.CurrentScreenPos = 0;
}

FGGPONetworkStats AFighterGameState::GetNetworkStats()
{
	AFighterMultiplayerRunner* Runner = Cast<AFighterMultiplayerRunner>(FighterRunner);
	if (IsValid(Runner))
	{
		FGGPONetworkStats Stats = { 0 };
		if (Runner->Players[0]->type == GGPO_PLAYERTYPE_REMOTE)
			GGPONet::ggpo_get_network_stats(Runner->ggpo, Runner->PlayerHandles[0], &Stats);
		else
			GGPONet::ggpo_get_network_stats(Runner->ggpo, Runner->PlayerHandles[1], &Stats);
		return Stats;
	}
	FGGPONetworkStats Stats = { 0 };
	return Stats;
}

void AFighterGameState::SortObjects()
{
	ActiveObjectCount = 6;
	for (int i = 6; i < 406; i++)
	{
		for (int j = i + 1; j < 406; j++)
		{
			if (SortedObjects[j]->IsActive && !SortedObjects[i]->IsActive)
			{
				ABattleActor* Temp = SortedObjects[i];
				SortedObjects[i] = SortedObjects[j];
				SortedObjects[j] = Temp;
			}
		}
		if (SortedObjects[i]->IsActive)
		{
			ActiveObjectCount++;
		}
	}
}

void AFighterGameState::SetOtherChecksum(uint32 RemoteChecksum, int32 InFrame)
{
	OtherChecksum = RemoteChecksum;
	OtherChecksumFrame = InFrame;
}

uint32 rollback_checksum(unsigned char* data, int size)
{
	uint32 c = 0;
	for(int i = 0; i < size; i++) {
		c = data[i] + 137 * c;
	}
	return c;
}

FRollbackData::FRollbackData()
{
	Checksum = 0;
	ActiveObjectCount = 0;
}

void AFighterGameState::Init()
{
	for (int i = 0; i < MAX_ROLLBACK_FRAMES; i++)
	{
		RollbackData.Add(FRollbackData());
	}
	UFighterGameInstance* GameInstance = Cast<UFighterGameInstance>(GetGameInstance());
	for (int i = 0; i < 6; i++)
	{
		if (GameInstance != nullptr)
		{
			if (GameInstance->PlayerList.Num() > i)
			{
				if (GameInstance->PlayerList[i] != nullptr)
				{
					Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(GameInstance->PlayerList[i]);
					Players[i]->PlayerIndex = i * 3 > 6;
					for (int j = 0; j < i; j++)
					{
						if (IsValid(GameInstance->PlayerList[j]))
						{
							if (Players[i]->IsA(GameInstance->PlayerList[j]))
							{
								Players[i]->ColorIndex = 2;
								break;
							}
						}
					}
				}
				else
				{
					Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(APlayerCharacter::StaticClass());
				}
				if (i % 3 == 0)
				{
					Players[i]->IsOnScreen = true;
				}
				if (i == 3 && GameInstance->IsCPUBattle)
				{
					AIController = GetWorld()->SpawnActor<AFighterAIController>(AIControllerClass);
					AIController->Possess(Players[i]);
				}
			}
			else
			{
				Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(APlayerCharacter::StaticClass());
				if (i % 3 == 0)
				{
					Players[i]->IsOnScreen = true;
				}
			}
			SortedObjects[i] = Players[i];
		}
		else
		{
			Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(APlayerCharacter::StaticClass());
			if (i % 3 == 0)
			{
				Players[i]->IsOnScreen = true;
			}
			SortedObjects[i] = Players[i];
		}
		Players[i]->InitPlayer();
		Players[i]->GameState = this;
		Players[i]->ObjNumber = i + 400;
	}
	for (int i = 0; i < 400; i++)
	{
		Objects[i] = GetWorld()->SpawnActor<ABattleActor>(ABattleActor::StaticClass());
		Objects[i]->ResetObject();
		Objects[i]->GameState = this;
		Objects[i]->ObjNumber = i;
		SortedObjects[i + 6] = Objects[i];
	}

	if(GameInstance)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwner();

		switch (GameInstance->FighterRunner)
		{
		case EFighterRunners::LocalPlay:
			FighterRunner = GetWorld()->SpawnActor<AFighterLocalRunner>(AFighterLocalRunner::StaticClass(),SpawnParameters);
			break;
		case EFighterRunners::MULTIPLAYER:
			FighterRunner = GetWorld()->SpawnActor<AFighterLocalRunner>(AFighterMultiplayerRunner::StaticClass(),SpawnParameters);
			break;
		case EFighterRunners::SYNCTEST:
			FighterRunner = GetWorld()->SpawnActor<AFighterLocalRunner>(AFighterSynctestRunner::StaticClass(),SpawnParameters);
			break;
		default:
			FighterRunner = GetWorld()->SpawnActor<AFighterLocalRunner>(AFighterLocalRunner::StaticClass(),SpawnParameters);
			break;
		}

		BattleState.RoundFormat = GameInstance->RoundFormat;
		BattleState.RoundTimer = GameInstance->StartRoundTimer * 60;
	}
	RoundInit();
}

void AFighterGameState::Update(int Input1, int Input2)
{
	if (UFighterGameInstance* GameInstance = Cast<UFighterGameInstance>(GetGameInstance()); !GameInstance->IsTraining && !BattleState.PauseTimer)
	{
		BattleState.RoundTimer--;
	}
	if (BattleState.RoundTimer < 0)
		BattleState.RoundTimer = 0;
	BattleState.FrameNumber++;

	for (int i = 0; i < 2; i++)
	{
		if (BattleState.Meter[i] > BattleState.MaxMeter[i])
			BattleState.Meter[i] = BattleState.MaxMeter[i];
		if (BattleState.Meter[i] < 0)
			BattleState.Meter[i] = 0;
	}
	
	if (!BattleState.PauseParticles)
		ParticleManager->UpdateParticles();

	SortObjects();
	Players[0]->Inputs = Input1;
	Players[3]->Inputs = Input2;
	for (int i = 0; i < 6; i++)
	{
		if (Players[i]->IsOnScreen)
		{
			for (int j = 0; j < 6; j++)
			{
				if (i < 3)
				{
					if (j >= 3 && Players[j]->IsOnScreen)
					{
						Players[i]->Enemy = Players[j];
					}
				}
				else
				{
					if (j < 3 && Players[j]->IsOnScreen)
					{
						Players[i]->Enemy = Players[j];
					}
				}
			}
		}
	}
	for (int i = 0; i < 406; i++)
	{
		if (i == ActiveObjectCount)
			break;
		if (!SortedObjects[i]->IsPlayer || SortedObjects[i]->Player->IsOnScreen)
			SortedObjects[i]->Update();
#ifdef SYNC_TEST
		if (SortedObjects[i]->Player->IsOnScreen)
		{
			UE_LOG(LogTemp, Warning, TEXT("Logging object %d"), i)
			SortedObjects[i]->LogForSyncTest();
		}
#endif
	}
	HandlePushCollision();
	HandleHitCollision();
	CollisionView();
	SetWallCollision();
	SetScreenBounds();
	HandleRoundWin();
	ManageAudio();
}

void AFighterGameState::SaveGameState()
{	int BackupFrame = LocalFrame % MAX_ROLLBACK_FRAMES;
	RollbackData[BackupFrame].ActiveObjectCount = ActiveObjectCount;
	RollbackData[BackupFrame].Checksum = BattleState.FrameNumber + BattleState.CurrentScreenPos + ActiveObjectCount;
	memcpy(RollbackData[BackupFrame].BattleStateBuffer, &BattleState.BattleStateSync, SIZEOF_BATTLESTATE);
	RollbackData[BackupFrame].Checksum += rollback_checksum(RollbackData[BackupFrame].BattleStateBuffer, SIZEOF_BATTLESTATE);
	for (int i = 0; i < 400; i++)
	{
		if (Objects[i]->IsActive)
		{
			Objects[i]->SaveForRollback(RollbackData[BackupFrame].ObjBuffer[i]);
			RollbackData[BackupFrame].ObjActive[i] = true;
		}
		else
			RollbackData[BackupFrame].ObjActive[i] = false;
		RollbackData[BackupFrame].Checksum += rollback_checksum(RollbackData[BackupFrame].ObjBuffer[i], SIZEOF_BATTLEACTOR);
		RollbackData[BackupFrame].Checksum += RollbackData[BackupFrame].ObjActive[i];
	}
	for (int i = 0; i < 6; i++)
	{
		Players[i]->SaveForRollback(RollbackData[BackupFrame].ObjBuffer[i + 400]);
		Players[i]->SaveForRollbackPlayer(RollbackData[BackupFrame].CharBuffer[i]);
		RollbackData[BackupFrame].Checksum += rollback_checksum(RollbackData[BackupFrame].ObjBuffer[i + 400], SIZEOF_BATTLEACTOR);
		RollbackData[BackupFrame].Checksum += rollback_checksum(RollbackData[BackupFrame].CharBuffer[i], SIZEOF_PLAYERCHARACTER);
	}
}

void AFighterGameState::LoadGameState()
{
	int CurrentRollbackFrame = SyncFrame % MAX_ROLLBACK_FRAMES;
	ActiveObjectCount = RollbackData[CurrentRollbackFrame].ActiveObjectCount;
	memcpy(&BattleState.BattleStateSync, RollbackData[CurrentRollbackFrame].BattleStateBuffer, SIZEOF_BATTLESTATE);
	for (int i = 0; i < 400; i++)
	{
		if (RollbackData[CurrentRollbackFrame].ObjActive[i])
		{
			Objects[i]->LoadForRollback(RollbackData[CurrentRollbackFrame].ObjBuffer[i]);
		}
		else
		{
			if (Objects[i]->IsActive)
				Objects[i]->ResetObject();
		}
	}
	for (int i = 0; i < 6; i++)
	{
		Players[i]->LoadForRollback(RollbackData[CurrentRollbackFrame].ObjBuffer[i + 400]);
		Players[i]->LoadForRollbackPlayer(RollbackData[CurrentRollbackFrame].CharBuffer[i]);
	}
	ParticleManager->RollbackParticles(LocalFrame - SyncFrame);
	SortObjects();
	RollbackStartAudio();
}

void AFighterGameState::HandlePushCollision()
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if (Players[i]->PlayerIndex != Players[j]->PlayerIndex && Players[i]->IsOnScreen && Players[j]->IsOnScreen)
			{
				Players[i]->HandlePushCollision(Players[j]);
			}
		}
	}
}

void AFighterGameState::HandleHitCollision()
{
	for (int i = 0; i < 406; i++)
	{
		if (i == ActiveObjectCount)
			break;
		for (int j = 0; j < 6; j++)
		{
			if (i != j && SortedObjects[j]->Player->IsOnScreen)
			{
				SortedObjects[i]->HandleHitCollision(Cast<APlayerCharacter>(SortedObjects[j]));
			}
		}
		for (int j = 0; j < 406; j++)
		{
			if (i != j)
			{
				SortedObjects[i]->HandleClashCollision(SortedObjects[j]);
			}
		}
	}
}

void AFighterGameState::UpdateCamera()
{
	if (CameraActor != nullptr)
	{
		float Distance = sqrt(abs((Players[0]->GetActorLocation() - Players[3]->GetActorLocation()).Y));
		Distance = FMath::Clamp(Distance,15, 25);
		float NewX = FMath::GetMappedRangeValueClamped(TRange<float>(0, 25), TRange<float>(0, 1080), Distance);
		FVector Average = (Players[0]->GetActorLocation() + Players[3]->GetActorLocation()) / 2;
		float NewY = FMath::Clamp(Average.Y,-700, 700);
		float NewZ = Average.Z + 150;
		FVector NewCameraLocation = FMath::Lerp(CameraActor->GetActorLocation(), FVector(-NewX, NewY, NewZ), 0.15);
		CameraActor->SetActorLocation(NewCameraLocation);
		if (!SequenceActor->SequencePlayer->IsPlaying())
		{
			SequenceCameraActor->SetActorLocation(FVector(-1080, 0, 175));
		}
	}else{
		for (TActorIterator<ACameraActor> It(GetWorld()); It;++It)
       	{
       		if(It->GetName().Contains("FighterCamera"))
       		{
       			CameraActor = (*It);
       			return;
       		}
       	}
		UFighterGameInstance* FGI = Cast<UFighterGameInstance>(GetGameInstance());
		if(FGI&& FGI->FighterCameraActor){
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetOwner();
			ACameraActor* fighterCamera = GetWorld()->SpawnActor<ACameraActor>(FGI->FighterCameraActor.Get()->StaticClass(),SpawnParameters);
			CameraActor=fighterCamera;
		}
	}
}

void AFighterGameState::UpdateUI()
{
	if (BattleUIActor != nullptr)
	{
		if (BattleUIActor->Widget != nullptr)
		{
			if (BattleUIActor->Widget->P1Health.Num() >= 3)
			{
				BattleUIActor->Widget->P1Health[0] = static_cast<float>(Players[0]->CurrentHealth) / static_cast<float>(Players[0]->Health);
				BattleUIActor->Widget->P1Health[1] = static_cast<float>(Players[1]->CurrentHealth) / static_cast<float>(Players[1]->Health);
				BattleUIActor->Widget->P1Health[2] = static_cast<float>(Players[2]->CurrentHealth) / static_cast<float>(Players[2]->Health);
			}
			if (BattleUIActor->Widget->P2Health.Num() >= 3)
			{
				BattleUIActor->Widget->P2Health[0] = static_cast<float>(Players[3]->CurrentHealth) / static_cast<float>(Players[3]->Health);
				BattleUIActor->Widget->P2Health[1] = static_cast<float>(Players[4]->CurrentHealth) / static_cast<float>(Players[4]->Health);
				BattleUIActor->Widget->P2Health[2] = static_cast<float>(Players[5]->CurrentHealth) / static_cast<float>(Players[5]->Health);
			}
			BattleUIActor->Widget->P1RoundsWon = BattleState.P1RoundsWon;
			BattleUIActor->Widget->P2RoundsWon = BattleState.P2RoundsWon;
			BattleUIActor->Widget->Timer = ceil(static_cast<float>(BattleState.RoundTimer) / 60);
			BattleUIActor->Widget->P1Meter = static_cast<float>(BattleState.Meter[0]) / 10000;
			BattleUIActor->Widget->P2Meter = static_cast<float>(BattleState.Meter[1]) / 10000;
			BattleUIActor->Widget->P1Gauge2 = static_cast<float>(BattleState.UniversalGauge[0]) / static_cast<float>(BattleState.MaxUniversalGauge);
			BattleUIActor->Widget->P2Gauge2 = static_cast<float>(BattleState.UniversalGauge[1]) / static_cast<float>(BattleState.MaxUniversalGauge);
			BattleUIActor->Widget->P1ComboCounter = Players[0]->ComboCounter;
			BattleUIActor->Widget->P2ComboCounter = Players[3]->ComboCounter;
		}
	}
}

void AFighterGameState::PlayLevelSequence(APlayerCharacter* Target, ULevelSequence* Sequence)
{
	if (SequenceActor != nullptr)
	{
		SequenceActor->SetSequence(Sequence);
		TArray<FMovieSceneBinding> Bindings = Sequence->GetMovieScene()->GetBindings();
		int NumBindings = Bindings.Num();
		for (int i = 0; i < NumBindings; i++)
		{
			FMovieSceneBinding MovieSceneBinding = Bindings[i];
			if (!MovieSceneBinding.GetName().Equals("Target"))
			{
				continue;
			}

			FMovieSceneObjectBindingID BindingId = FMovieSceneObjectBindingID(MovieSceneBinding.GetObjectGuid());
			SequenceActor->SetBinding(BindingId, TArray<AActor*>{ Target });

			break;
		}
		for (int i = 0; i < NumBindings; i++)
		{
			FMovieSceneBinding MovieSceneBinding = Bindings[i];
			if (!MovieSceneBinding.GetName().Equals("BP_FighterCamera"))
			{
				continue;
			}

			FMovieSceneObjectBindingID BindingId = FMovieSceneObjectBindingID(MovieSceneBinding.GetObjectGuid());
			SequenceActor->SetBinding(BindingId, TArray<AActor*>{ CameraActor });

			break;
		}
		for (int i = 0; i < NumBindings; i++)
		{
			FMovieSceneBinding MovieSceneBinding = Bindings[i];
			if (!MovieSceneBinding.GetName().Equals("BP_SequenceCamera"))
			{
				continue;
			}

			FMovieSceneObjectBindingID BindingId = FMovieSceneObjectBindingID(MovieSceneBinding.GetObjectGuid());
			SequenceActor->SetBinding(BindingId, TArray<AActor*>{ SequenceCameraActor });

			break;
		}
		SequenceTarget = Target;
		SequenceActor->SequencePlayer->Stop();
		SequenceActor->SequencePlayer->Play();
	}
}

ABattleActor* AFighterGameState::AddBattleActor(UState* InState, int PosX, int PosY, bool FacingRight, APlayerCharacter* Parent)
{
	for (int i = 0; i < 400; i++)
	{
		if (!Objects[i]->IsActive)
		{
			Objects[i]->ObjectState = DuplicateObject(InState, Objects[i]);
			Objects[i]->ObjectState->ObjectParent = Objects[i];
			Objects[i]->IsActive = true;
			Objects[i]->FacingRight = FacingRight;
			Objects[i]->Player = Parent;
			Objects[i]->SetPosX(PosX);
			Objects[i]->SetPosY(PosY);
			Objects[i]->InitObject();
			return Objects[i];
		}
	}
	return nullptr;
}

void AFighterGameState::CollisionView()
{
	if (DisplayCollision)
	{
		for (int i = 0; i < 6; i++)
		{
			if (Players[i]->IsOnScreen)
			{
				SortedObjects[i]->CollisionView();
			}
		}
		for (int i = 6; i < 406; i++)
		{
			if (i == ActiveObjectCount)
				break;
			SortedObjects[i]->CollisionView();
		}
	}
}

void AFighterGameState::StartSuperFreeze(int Duration)
{
	for (int i = 0; i < ActiveObjectCount; i++)
		SortedObjects[i]->SuperFreezeTime = Duration;
	BattleState.PauseTimer = true;
}

void AFighterGameState::BattleHudVisibility(bool Visible)
{
	if (Visible)
		BattleUIActor->Widget->SetVisibility(ESlateVisibility::Visible);
	else
		BattleUIActor->Widget->SetVisibility(ESlateVisibility::Hidden);
}

void AFighterGameState::PlayCommonAudio(USoundBase* InSoundWave, float MaxDuration)
{
	for (int i = 0; i < CommonAudioChannelCount; i++)
	{
		if (BattleState.CommonAudioChannels[i].Finished)
		{
			BattleState.CommonAudioChannels[i].SoundWave = InSoundWave;
			BattleState.CommonAudioChannels[i].StartingFrame = BattleState.FrameNumber;
			BattleState.CommonAudioChannels[i].MaxDuration = MaxDuration;
			BattleState.CommonAudioChannels[i].Finished = false;
			AudioManager->CommonAudioPlayers[i]->SetSound(InSoundWave);
			AudioManager->CommonAudioPlayers[i]->Play();
			return;
		}
	}
}

void AFighterGameState::PlayCharaAudio(USoundBase* InSoundWave, float MaxDuration)
{
	for (int i = 0; i < CharaAudioChannelCount; i++)
	{
		if (BattleState.CharaAudioChannels[i].Finished)
		{
			BattleState.CharaAudioChannels[i].SoundWave = InSoundWave;
			BattleState.CharaAudioChannels[i].StartingFrame = BattleState.FrameNumber;
			BattleState.CharaAudioChannels[i].MaxDuration = MaxDuration;
			BattleState.CharaAudioChannels[i].Finished = false;
			AudioManager->CharaAudioPlayers[i]->SetSound(InSoundWave);
			AudioManager->CharaAudioPlayers[i]->Play();
			return;
		}
	}
}

void AFighterGameState::PlayVoiceLine(USoundBase* InSoundWave, float MaxDuration, int Player)
{
	BattleState.CharaVoiceChannels[Player].SoundWave = InSoundWave;
	BattleState.CharaVoiceChannels[Player].StartingFrame = BattleState.FrameNumber;
	BattleState.CharaVoiceChannels[Player].MaxDuration = MaxDuration;
	BattleState.CharaVoiceChannels[Player].Finished = false;
	AudioManager->CharaVoicePlayers[Player]->SetSound(InSoundWave);
	AudioManager->CharaVoicePlayers[Player]->Play();
}

void AFighterGameState::ManageAudio()
{
	for (int i = 0; i < CommonAudioChannelCount; i++)
	{
		int CurrentAudioTime = BattleState.FrameNumber - BattleState.CommonAudioChannels[i].StartingFrame;
		if (!BattleState.CommonAudioChannels[i].Finished && static_cast<int>(BattleState.CommonAudioChannels[i].MaxDuration * 60) < CurrentAudioTime + 0.2)
		{
			BattleState.CommonAudioChannels[i].Finished = true;
			AudioManager->CommonAudioPlayers[i]->Stop();
			AudioManager->CommonAudioPlayers[i]->SetSound(nullptr);
		}
	}
	for (int i = 0; i < CharaAudioChannelCount; i++)
	{
		int CurrentAudioTime = BattleState.FrameNumber - BattleState.CharaAudioChannels[i].StartingFrame;
		if (!BattleState.CharaAudioChannels[i].Finished && static_cast<int>(BattleState.CharaAudioChannels[i].MaxDuration * 60) < CurrentAudioTime + 0.2)
		{
			BattleState.CharaAudioChannels[i].Finished = true;
			AudioManager->CharaAudioPlayers[i]->Stop();
			AudioManager->CharaAudioPlayers[i]->SetSound(nullptr);
		}
	}
	for (int i = 0; i < CharaVoiceChannelCount; i++)
	{
		int CurrentAudioTime = BattleState.FrameNumber - BattleState.CharaVoiceChannels[i].StartingFrame;
		if (!BattleState.CharaVoiceChannels[i].Finished && static_cast<int>(BattleState.CharaVoiceChannels[i].MaxDuration * 60) < CurrentAudioTime + 0.2)
		{
			BattleState.CharaVoiceChannels[i].Finished = true;
			AudioManager->CharaVoicePlayers[i]->Stop();
			AudioManager->CharaVoicePlayers[i]->SetSound(nullptr);
		}
	}
	int CurrentAudioTime = BattleState.FrameNumber - BattleState.AnnouncerVoiceChannel.StartingFrame;
	if (!BattleState.AnnouncerVoiceChannel.Finished && static_cast<int>(BattleState.AnnouncerVoiceChannel.MaxDuration * 60) < CurrentAudioTime + 0.2)
	{
		BattleState.AnnouncerVoiceChannel.Finished = true;
		AudioManager->AnnouncerVoicePlayer->Stop();
		AudioManager->AnnouncerVoicePlayer->SetSound(nullptr);
	}
}

void AFighterGameState::RollbackStartAudio()
{
	for (int i = 0; i < CommonAudioChannelCount; i++)
	{
		if (BattleState.CommonAudioChannels[i].SoundWave != AudioManager->CommonAudioPlayers[i]->GetSound())
		{
			AudioManager->CommonAudioPlayers[i]->Stop();
			AudioManager->CommonAudioPlayers[i]->SetSound(BattleState.CommonAudioChannels[i].SoundWave);
			float CurrentAudioTime = float(BattleState.FrameNumber - BattleState.CommonAudioChannels[i].StartingFrame) / 60.f;
			if (!BattleState.CommonAudioChannels[i].Finished && !AudioManager->CommonAudioPlayers[i]->IsPlaying())
			{
				//AudioManager->CommonAudioPlayers[i]->SetFloatParameter(FName(TEXT("Start Time")), CurrentAudioTime);
				AudioManager->CommonAudioPlayers[i]->Play(CurrentAudioTime);
			}
		}
	}
	for (int i = 0; i < CharaAudioChannelCount; i++)
	{
		if (BattleState.CharaAudioChannels[i].SoundWave != AudioManager->CharaAudioPlayers[i]->GetSound())
		{
			AudioManager->CharaAudioPlayers[i]->Stop();
			AudioManager->CharaAudioPlayers[i]->SetSound(BattleState.CharaAudioChannels[i].SoundWave);
			float CurrentAudioTime = float(BattleState.FrameNumber - BattleState.CharaAudioChannels[i].StartingFrame) / 60.f;
			if (!BattleState.CharaAudioChannels[i].Finished && !AudioManager->CharaAudioPlayers[i]->IsPlaying())
			{
				//AudioManager->CharaAudioPlayers[i]->SetFloatParameter(FName(TEXT("Start Time")), CurrentAudioTime);
				AudioManager->CharaAudioPlayers[i]->Play(CurrentAudioTime);
			}
		}
	}
	for (int i = 0; i < CharaVoiceChannelCount; i++)
	{
		if (BattleState.CharaVoiceChannels[i].SoundWave != AudioManager->CharaVoicePlayers[i]->GetSound())
		{
			AudioManager->CharaVoicePlayers[i]->Stop();
			AudioManager->CharaVoicePlayers[i]->SetSound(BattleState.CharaVoiceChannels[i].SoundWave);
			float CurrentAudioTime = float(BattleState.FrameNumber - BattleState.CharaVoiceChannels[i].StartingFrame) / 60.f;
			if (!BattleState.CharaVoiceChannels[i].Finished && !AudioManager->CharaVoicePlayers[i]->IsPlaying())
			{
				//AudioManager->CharaVoicePlayers[i]->SetFloatParameter(FName(TEXT("Start Time")), CurrentAudioTime);
				AudioManager->CharaVoicePlayers[i]->Play(CurrentAudioTime);
			}
		}
	}
	if (BattleState.AnnouncerVoiceChannel.SoundWave != AudioManager->AnnouncerVoicePlayer->GetSound())
	{
		AudioManager->AnnouncerVoicePlayer->Stop();
		AudioManager->AnnouncerVoicePlayer->SetSound(BattleState.AnnouncerVoiceChannel.SoundWave);
		float CurrentAudioTime = float(BattleState.FrameNumber - BattleState.AnnouncerVoiceChannel.StartingFrame) / 60.f;
		if (!BattleState.AnnouncerVoiceChannel.Finished && !AudioManager->AnnouncerVoicePlayer->IsPlaying())
		{
			//AudioManager->AnnouncerVoicePlayer->SetFloatParameter(FName(TEXT("Start Time")), CurrentAudioTime);
			AudioManager->AnnouncerVoicePlayer->Play(CurrentAudioTime);
		}
	}
}

void AFighterGameState::SetWallCollision()
{
	for (int i = 0; i < 6; i++)
	{
		if (Players[i] != nullptr)
		{
			if (Players[i]->IsOnScreen)
			{
				Players[i]->TouchingWall = true;
				if (Players[i]->GetInternalValue(VAL_PosX) >= 1080000 + BattleState.CurrentScreenPos)
				{
					Players[i]->SetPosX(1080000 + BattleState.CurrentScreenPos);
				}
				else if (Players[i]->GetInternalValue(VAL_PosX) <= -1080000 + BattleState.CurrentScreenPos)
				{
					Players[i]->SetPosX(-1080000 + BattleState.CurrentScreenPos);
				}
				else if (Players[i]->GetInternalValue(VAL_PosX) < 1080000 + BattleState.CurrentScreenPos || Players[i]->GetInternalValue(VAL_PosX) > -1080000 + BattleState.CurrentScreenPos)
				{
					Players[i]->TouchingWall = false;
				}
			}
		}
	}
}

void AFighterGameState::SetScreenBounds()
{
	for (int i = 0; i < 6; i++)
	{
		if (Players[i]->PlayerIndex == 0)
		{
			for (int j = 0; j < 6; j++)
			{
				if (Players[j]->PlayerIndex == 1)
				{
					if (Players[i]->IsOnScreen && Players[j]->IsOnScreen)
					{
					    int NewScreenPos = (Players[i]->GetInternalValue(VAL_PosX) + Players[j]->GetInternalValue(VAL_PosX)) / 2;
						BattleState.CurrentScreenPos = BattleState.CurrentScreenPos + (NewScreenPos - BattleState.CurrentScreenPos) * 5 / 100;
						if (BattleState.CurrentScreenPos > 1080000)
						{
							BattleState.CurrentScreenPos = 1080000;
						}
						else if (BattleState.CurrentScreenPos < -1080000)
						{
							BattleState.CurrentScreenPos = -1080000;
						}
					}
				}
			}
		}
	}
}
