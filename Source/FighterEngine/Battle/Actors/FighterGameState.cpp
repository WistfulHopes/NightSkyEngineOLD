#include "FighterGameState.h"
#include "DefaultLevelSequenceInstanceData.h"
#include "EngineUtils.h"
#include "FighterEngine/Miscellaneous/BattleUIActor.h"
#include "Net/UnrealNetwork.h"
#include "FighterEngine/Miscellaneous/FighterGameInstance.h"
#include "FighterPlayerController.h"
#include "LevelSequenceActor.h"
#include "Camera/CameraActor.h"
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
	if (GetWorld()->GetNetMode() == NM_Standalone)
		UGameplayStatics::CreatePlayer(GWorld);
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

	PlayAllAudio();
	UpdateCamera();
	UpdateUI();
}

void AFighterGameState::TickGameState()
{
	if (SyncTestError)
		return;
#ifdef SYNC_TEST
	RemoteFrame++;
#else
	{
		RemoteFrame++;
		LocalFrame++;
		UpdateLocalInput();
		Update(LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0], LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1]);
		return;
	}
#endif
// 	if (FrameNumber < INITIAL_FRAME)
// 	{
// 		Update(16, 16);
// 		SaveGameState();
// 		return;
// 	}
// 	const int PlayerIndex = Cast<UFighterGameInstance>(GetGameInstance())->PlayerIndex;
// 	int FinalFrame = RemoteFrame;
// 	if (RemoteFrame > LocalFrame)
// 		FinalFrame = LocalFrame;
// #ifdef SYNC_TEST
// 	if (FinalFrame % 2)
// 		SyncFrame = FinalFrame;
// 	Checksum = RollbackData[FrameNumber % MAX_ROLLBACK_FRAMES].Checksum;
// #else
// 	for (int i = FinalFrame - MAX_ROLLBACK_FRAMES + abs(LocalFrame - RemoteFrame) + 1; i <= FinalFrame; i++)
// 	{
// 		SyncFrame = i - 1;
// 		if (LocalInputs[i % MAX_ROLLBACK_FRAMES][(PlayerIndex + 1) % 2] != RemoteInputs[i % MAX_ROLLBACK_FRAMES][(PlayerIndex + 1) % 2])
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("SyncFrame %d, LocalFrame %d, RemoteFrame %d, FrameNumber %d"), SyncFrame, LocalFrame, RemoteFrame, FrameNumber)
// 			break;
// 		}
// 		if (i == FinalFrame)
// 			SyncFrame = FinalFrame;
// 	}
// #endif
// 	if (NeedRollback())
// 	{
// 		LoadGameState();
// 		uint32 LoadChecksum = RollbackData[FrameNumber % MAX_ROLLBACK_FRAMES].Checksum;
// 		SaveGameState();
// 		if (LoadChecksum != RollbackData[FrameNumber % MAX_ROLLBACK_FRAMES].Checksum)
// 		{
// 			UE_LOG(LogTemp, Error, TEXT("Rollback checksum failed for frame %d! Saved checksum %d, this checksum %d! Game cannot proceed."), FrameNumber,
// 				RollbackData[FrameNumber % MAX_ROLLBACK_FRAMES].Checksum, LoadChecksum)
// 			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
// 			SyncTestError = true;
// 		}
// #ifndef SYNC_TEST
// 		FMemory::Memcpy(LocalInputs, RemoteInputs, sizeof RemoteInputs);
// 		UE_LOG(LogTemp, Warning, TEXT("P1 previous input %d, P2 previous input %d, frame %d"), Players[0]->InputBuffer.InputBufferInternal[88],	Players[3]->InputBuffer.InputBufferInternal[88], FrameNumber)
// #endif
// 		for (int i = SyncFrame + 1; i <= LocalFrame; i++)
// 		{
// #ifdef SYNC_TEST
// 			UE_LOG(LogTemp, Warning, TEXT("SyncTest resimulation log for frame # %d"), i)
// 			UE_LOG(LogTemp, Warning, TEXT("FrameNumber: %d"), FrameNumber)
// 			UE_LOG(LogTemp, Warning, TEXT("BattleState.CurrentScreenPos: %d"), BattleState.CurrentScreenPos)
// 			UE_LOG(LogTemp, Warning, TEXT("ActiveObjectCount: %d"), ActiveObjectCount)
// #endif
// 			if (i > RemoteFrame)
// 			{
// 				if (PlayerIndex == 0)
// 					LocalInputs[i % MAX_ROLLBACK_FRAMES][1] = LocalInputs[RemoteFrame % MAX_ROLLBACK_FRAMES][1];
// 				else
// 					LocalInputs[i % MAX_ROLLBACK_FRAMES][0] = LocalInputs[RemoteFrame % MAX_ROLLBACK_FRAMES][0];
// 			}
// 			UE_LOG(LogTemp, Warning, TEXT("P1 input %d, P2 input %d for frame %d"), LocalInputs[i % MAX_ROLLBACK_FRAMES][0], LocalInputs[i % MAX_ROLLBACK_FRAMES][1], FrameNumber + 1)
// 			Update(LocalInputs[(i - FRAME_DELAY) % MAX_ROLLBACK_FRAMES][0], LocalInputs[(i - FRAME_DELAY) % MAX_ROLLBACK_FRAMES][1]);
// 			SaveGameState();
// 		}
// 		Cast<AFighterPlayerController>(	GetWorld()->GetFirstPlayerController())->CheckForDesyncs(RollbackData[LocalFrame % MAX_ROLLBACK_FRAMES].Checksum, LocalFrame);
// 		CheckForDesyncs();
// #ifdef SYNC_TEST
//         if (Checksum != RollbackData[FrameNumber % MAX_ROLLBACK_FRAMES].Checksum)
//         {
//         	UE_LOG(LogTemp, Error, TEXT("Failed to match checksum for frame %d! Saved checksum %d, this checksum %d! Game cannot proceed."), FrameNumber,
//         		RollbackData[FrameNumber % MAX_ROLLBACK_FRAMES].Checksum, Checksum)
//         	UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
//         	SyncTestError = true;
//         }
// 		else
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("Checksum log: Saved checksum %d, this checksum %d"),
// 			RollbackData[FrameNumber % MAX_ROLLBACK_FRAMES].Checksum, Checksum)
// 			SyncTestErrorCount = 0;
// 		}
// #endif
// 	}
// 	if (TimeSynced())
// 	{
// 		ReconnectTime = 0;
// 		LocalFrame++;
// #ifdef SYNC_TEST
// 		UE_LOG(LogTemp, Warning, TEXT("SyncTest log for frame # %d"), LocalFrame)
// 		UE_LOG(LogTemp, Warning, TEXT("FrameNumber: %d"), FrameNumber)
// 		UE_LOG(LogTemp, Warning, TEXT("BattleState.CurrentScreenPos: %d"), BattleState.CurrentScreenPos)
// #endif
// 		UpdateLocalInput();
// 		Update(LocalInputs[(LocalFrame - FRAME_DELAY) % MAX_ROLLBACK_FRAMES][0], LocalInputs[(LocalFrame - FRAME_DELAY) % MAX_ROLLBACK_FRAMES][1]);
// 		SaveGameState();
// 	}
// 	else
// 	{
// 		ReconnectTime++;
// 		if (ReconnectTime > 60)
// 		{
// 			UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
// 			SyncTestError = true;
// 		}
// 	}
}

int AFighterGameState::GetLocalInputs(int Index)
{
	if (const AFighterPlayerController* Controller = Cast<AFighterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), Index)); Controller != nullptr)
	{
		return Controller->Inputs;
	}
	return 0;
}

void AFighterGameState::UpdateLocalInput()
{
#ifdef SYNC_TEST
	LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0] = rand();
	LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1] = rand();
	RemoteInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0] = rand();
	RemoteInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1] = rand();
#else
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0] = GetLocalInputs(0);
		LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1] = InputNeutral;
		if (GetWorld()->GetAuthGameMode()->GetNumPlayers() > 1)
		{
			LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1] = GetLocalInputs(1);
		}
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
	UE_LOG(LogTemp, Warning, TEXT("P1 input %d, P2 input %d for frame %d"), LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][0], LocalInputs[LocalFrame % MAX_ROLLBACK_FRAMES][1], LocalFrame)
#endif
}

void AFighterGameState::UpdateRemoteInput(int RemoteInput[], int32 InFrame, int32 InFrameAdvantage)
{
#ifndef SYNC_TEST
	if (InFrame >= LocalFrame + MAX_ROLLBACK_FRAMES)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Inputs arrived too late! Game cannot proceed."));
		UE_LOG(LogTemp, Error, TEXT("Inputs arrived too late! Game cannot proceed."))
		SyncTestError = true;
		UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		return;
	}
	if (InFrame > RemoteFrame + 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Inputs arrived in the wrong order! Game cannot proceed."));
		UE_LOG(LogTemp, Error, TEXT("Inputs arrived in the wrong order! Game cannot proceed."))
		SyncTestError = true;
		UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		return;
	}
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
#endif
}

bool AFighterGameState::NeedRollback()
{
	if (LocalFrame > SyncFrame && RemoteFrame > SyncFrame)
		return true;
	return false;
}

bool AFighterGameState::TimeSynced()
{
	LocalFrameAdvantage = LocalFrame - RemoteFrame;
	int FrameAdvantageDifference = LocalFrameAdvantage - RemoteFrameAdvantage;
	return LocalFrameAdvantage < MAX_ROLLBACK_FRAMES && FrameAdvantageDifference <= FRAME_ADVANTAGE_LIMIT;
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
			if (Players[0]->CurrentHealth > 0)
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
			else if (Players[3]->CurrentHealth > 0)
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

bool AFighterGameState::CheckForDesyncs()
{
	if (LocalFrame - MAX_ROLLBACK_FRAMES + 1 < OtherChecksumFrame && OtherChecksumFrame <= LocalFrame
		&& OtherChecksumFrame != PrevOtherChecksumFrame && RollbackData[OtherChecksumFrame % MAX_ROLLBACK_FRAMES].Checksum != OtherChecksum)
	{
		SyncTestErrorCount++;
		UE_LOG(LogTemp, Warning, TEXT("In frame %d, desync count %d, local frame %d, remote checksum %d, local checksum %d"), OtherChecksumFrame,
			SyncTestErrorCount, LocalFrame, OtherChecksum, RollbackData[OtherChecksumFrame % MAX_ROLLBACK_FRAMES].Checksum)
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Desync detected! Game cannot proceed. Press ALT+F4 to exit."));
		UE_LOG(LogTemp, Error, TEXT("Desync detected! Game cannot proceed."))
		SyncTestError = true;
		UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
		return true;
	}
	PrevOtherChecksumFrame = OtherChecksumFrame;
	return true;
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
	FrameNumber = 0;
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
				Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(APlayerCharacter::StaticClass());

				if (GameInstance->PlayerList[i] != nullptr)
				{
					Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(GameInstance->PlayerList[i]);
					Players[i]->PlayerIndex = i * 3 > 6;
				}
				else
				{
					Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(APlayerCharacter::StaticClass());
				}
				if (i % 3 == 0)
				{
					Players[i]->IsOnScreen = true;
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
}

void AFighterGameState::Update(int Input1, int Input2)
{
	if (!BattleState.PauseTimer)
		BattleState.RoundTimer--;
	if (BattleState.RoundTimer < 0)
		BattleState.RoundTimer = 0;
	FrameNumber++;

	for (int i = 0; i < 2; i++)
	{
		if (BattleState.Meter[i] > BattleState.MaxMeter[i])
			BattleState.Meter[i] = BattleState.MaxMeter[i];
		if (BattleState.Meter[i] < 0)
			BattleState.Meter[i] = 0;
	}
	
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
}

void AFighterGameState::SaveGameState()
{	int BackupFrame = LocalFrame % MAX_ROLLBACK_FRAMES;
	RollbackData[BackupFrame].FrameNumber = FrameNumber;
	RollbackData[BackupFrame].ActiveObjectCount = ActiveObjectCount;
	RollbackData[BackupFrame].Checksum = FrameNumber + BattleState.CurrentScreenPos + ActiveObjectCount;
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
	FrameNumber = RollbackData[CurrentRollbackFrame].FrameNumber;
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
		Distance = FMath::Clamp(Distance,18, 25);
		float NewX = FMath::GetMappedRangeValueClamped(TRange<float>(0, 25), TRange<float>(0, 1080), Distance);
		FVector Average = (Players[0]->GetActorLocation() + Players[3]->GetActorLocation()) / 2;
		float NewY = FMath::Clamp(Average.Y,-630, 630);
		float NewZ = Average.Z + 175;
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
				BattleUIActor->Widget->P1Health[0] = float(Players[0]->CurrentHealth) / float(Players[0]->Health);
				BattleUIActor->Widget->P1Health[1] = float(Players[1]->CurrentHealth) / float(Players[1]->Health);
				BattleUIActor->Widget->P1Health[2] = float(Players[2]->CurrentHealth) / float(Players[2]->Health);
			}
			if (BattleUIActor->Widget->P2Health.Num() >= 3)
			{
				BattleUIActor->Widget->P2Health[0] = float(Players[3]->CurrentHealth) / float(Players[3]->Health);
				BattleUIActor->Widget->P2Health[1] = float(Players[4]->CurrentHealth) / float(Players[4]->Health);
				BattleUIActor->Widget->P2Health[2] = float(Players[5]->CurrentHealth) / float(Players[5]->Health);
			}
			BattleUIActor->Widget->P1RoundsWon = BattleState.P1RoundsWon;
			BattleUIActor->Widget->P2RoundsWon = BattleState.P2RoundsWon;
			BattleUIActor->Widget->Timer = ceil((float)BattleState.RoundTimer / 60);
			BattleUIActor->Widget->P1Meter = float(BattleState.Meter[0]) / 10000;
			BattleUIActor->Widget->P2Meter = float(BattleState.Meter[1]) / 10000;
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

void AFighterGameState::PlayCommonAudio(USoundWave* InSoundWave)
{
	for (int i = 0; i < CommonAudioChannelCount; i++)
	{
		if (BattleState.CommonAudioChannels[i].Finished)
		{
			BattleState.CommonAudioChannels[i].SoundWave = InSoundWave;
			BattleState.CommonAudioChannels[i].StartingFrame = FrameNumber;
			CommonAudioChannels[i].Finished = false;
			AudioManager->CommonAudioPlayers[i]->SetSound(nullptr);
			return;
		}
	}
}

void AFighterGameState::PlayCharaAudio(USoundWave* InSoundWave)
{
	for (int i = 0; i < CharaAudioChannelCount; i++)
	{
		if (BattleState.CharaAudioChannels[i].Finished)
		{
			BattleState.CharaAudioChannels[i].SoundWave = InSoundWave;
			BattleState.CharaAudioChannels[i].StartingFrame = FrameNumber;
			CharaAudioChannels[i].Finished = false;
			AudioManager->CharaAudioPlayers[i]->SetSound(nullptr);
			return;
		}
	}
}

void AFighterGameState::PlayVoiceLine(USoundWave* InSoundWave, int Player)
{
	BattleState.CharaVoiceChannels[Player].SoundWave = InSoundWave;
	BattleState.CharaVoiceChannels[Player].StartingFrame = FrameNumber;
	CharaVoiceChannels[Player].Finished = false;
	AudioManager->CharaVoicePlayers[Player]->SetSound(nullptr);
}

void AFighterGameState::PlayAllAudio()
{
	for (int i = 0; i < CommonAudioChannelCount; i++)
	{
		CommonAudioChannels[i].SoundWave = BattleState.CommonAudioChannels[i].SoundWave;
		CommonAudioChannels[i].StartingFrame = BattleState.CommonAudioChannels[i].StartingFrame;
		if (CommonAudioChannels[i].SoundWave != nullptr)
		{
			if (IsValid(AudioManager->CommonAudioPlayers[i]->Sound))
			{
				if (!AudioManager->CommonAudioPlayers[i]->IsPlaying())
				{
					BattleState.CommonAudioChannels[i].Finished = true;
					CommonAudioChannels[i].Finished = true;
					continue;
				}
				if (AudioManager->CommonAudioPlayers[i]->GetSound() != CommonAudioChannels[i].SoundWave && !CommonAudioChannels[i].Finished)
				{
					AudioManager->CommonAudioPlayers[i]->SetSound(CommonAudioChannels[i].SoundWave);
					float StartTime = float(FrameNumber - CommonAudioChannels[i].StartingFrame) / 60.;
					AudioManager->CommonAudioPlayers[i]->Play(StartTime);
					CommonAudioChannels[i].Finished = false;
				}
			}
			else if (!CommonAudioChannels[i].Finished)
			{
				AudioManager->CommonAudioPlayers[i]->SetSound(CommonAudioChannels[i].SoundWave);
				float StartTime = float(FrameNumber - CommonAudioChannels[i].StartingFrame) / 60.;
				AudioManager->CommonAudioPlayers[i]->Play(StartTime);
				CommonAudioChannels[i].Finished = false;
			}
		}
	}
	for (int i = 0; i < CharaAudioChannelCount; i++)
	{
		CharaAudioChannels[i].SoundWave = BattleState.CharaAudioChannels[i].SoundWave;
		CharaAudioChannels[i].StartingFrame = BattleState.CharaAudioChannels[i].StartingFrame;
		if (CharaAudioChannels[i].SoundWave != nullptr)
		{
			if (IsValid(AudioManager->CharaAudioPlayers[i]->Sound))
			{
				if (!AudioManager->CharaAudioPlayers[i]->IsPlaying())
				{
					BattleState.CharaAudioChannels[i].Finished = true;
					CharaAudioChannels[i].Finished = true;
					continue;
				}
				if (AudioManager->CharaAudioPlayers[i]->GetSound() != CharaAudioChannels[i].SoundWave && !CharaAudioChannels[i].Finished)
				{
					AudioManager->CharaAudioPlayers[i]->SetSound(CharaAudioChannels[i].SoundWave);
					float StartTime = float(FrameNumber - CharaAudioChannels[i].StartingFrame) / 60.;
					AudioManager->CharaAudioPlayers[i]->Play(StartTime);
					CharaAudioChannels[i].Finished = false;
				}
			}
			else if (!CharaAudioChannels[i].Finished)
			{
				AudioManager->CharaAudioPlayers[i]->SetSound(CharaAudioChannels[i].SoundWave);
				float StartTime = float(FrameNumber - CharaAudioChannels[i].StartingFrame) / 60.;
				AudioManager->CharaAudioPlayers[i]->Play(StartTime);
				CharaAudioChannels[i].Finished = false;
			}
		}
	}
	for (int i = 0; i < CharaVoiceChannelCount; i++)
	{
		CharaVoiceChannels[i].SoundWave = BattleState.CharaVoiceChannels[i].SoundWave;
		CharaVoiceChannels[i].StartingFrame = BattleState.CharaVoiceChannels[i].StartingFrame;
		if (CharaVoiceChannels[i].SoundWave != nullptr)
		{
			if (IsValid(AudioManager->CharaVoicePlayers[i]->Sound))
			{
				if (!AudioManager->CharaVoicePlayers[i]->IsPlaying())
				{
					BattleState.CharaVoiceChannels[i].Finished = true;
					CharaVoiceChannels[i].Finished = true;
					continue;
				}
				if (AudioManager->CharaVoicePlayers[i]->GetSound() != CharaVoiceChannels[i].SoundWave && !CharaVoiceChannels[i].Finished)
				{
					AudioManager->CharaVoicePlayers[i]->SetSound(CharaVoiceChannels[i].SoundWave);
					float StartTime = float(FrameNumber - CharaVoiceChannels[i].StartingFrame) / 60.;
					AudioManager->CharaVoicePlayers[i]->Play(StartTime);
					CharaVoiceChannels[i].Finished = false;
				}
			}
			else if (!CharaVoiceChannels[i].Finished)
			{
				AudioManager->CharaVoicePlayers[i]->SetSound(CharaVoiceChannels[i].SoundWave);
				float StartTime = float(FrameNumber - CharaVoiceChannels[i].StartingFrame) / 60.;
				AudioManager->CharaVoicePlayers[i]->Play(StartTime);
				UE_LOG(LogTemp, Warning, TEXT("%f"), StartTime)
				CharaVoiceChannels[i].Finished = false;
			}
		}
	}
	AnnouncerVoiceChannel.SoundWave = BattleState.AnnouncerVoiceChannel.SoundWave;
	AnnouncerVoiceChannel.StartingFrame = BattleState.AnnouncerVoiceChannel.StartingFrame;
	if (AnnouncerVoiceChannel.SoundWave != nullptr)
	{
		if (IsValid(AudioManager->AnnouncerVoicePlayer->Sound))
		{
			if (!AudioManager->AnnouncerVoicePlayer->IsPlaying())
			{
				BattleState.AnnouncerVoiceChannel.Finished = true;
				AnnouncerVoiceChannel.Finished = true;
				return;
			}
			if (AudioManager->AnnouncerVoicePlayer->GetSound() != AnnouncerVoiceChannel.SoundWave && !AnnouncerVoiceChannel.Finished)
			{
				AudioManager->AnnouncerVoicePlayer->SetSound(AnnouncerVoiceChannel.SoundWave);
				float StartTime = float(FrameNumber - AnnouncerVoiceChannel.StartingFrame) / 60.;
				AudioManager->AnnouncerVoicePlayer->Play(StartTime);
				AnnouncerVoiceChannel.Finished = false;
			}
		}
		else if (!AnnouncerVoiceChannel.Finished)
		{
			AudioManager->AnnouncerVoicePlayer->SetSound(AnnouncerVoiceChannel.SoundWave);
			float StartTime = float(FrameNumber - AnnouncerVoiceChannel.StartingFrame) / 60.;
			AudioManager->AnnouncerVoicePlayer->Play(StartTime);
			AnnouncerVoiceChannel.Finished = false;
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
				if (Players[i]->GetInternalValue(VAL_PosX) > 1080000 + BattleState.CurrentScreenPos)
				{
					Players[i]->SetPosX(1080001 + BattleState.CurrentScreenPos);
				}
				else if (Players[i]->GetInternalValue(VAL_PosX) < -1080000 + BattleState.CurrentScreenPos)
				{
					Players[i]->SetPosX(-1080001 + BattleState.CurrentScreenPos);
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
