// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterGameState.h"

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


//#define SYNC_TEST

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
}

void AFighterGameState::TickGameState()
{
	if (SyncTestError)
		return;
#ifdef SYNC_TEST
	RemoteFrame++;
#else
	//if (GetWorld()->GetNetMode() == NM_Standalone)
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
// 			UE_LOG(LogTemp, Warning, TEXT("CurrentScreenPos: %d"), CurrentScreenPos)
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
// 		UE_LOG(LogTemp, Warning, TEXT("CurrentScreenPos: %d"), CurrentScreenPos)
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
	ScreenPosition = 0;
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
				SortedObjects[i] = Players[i];
				Players[i]->Init();
				Players[i]->InitPlayer();
			}
			else
			{
				Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(APlayerCharacter::StaticClass());
				if (i % 3 == 0)
				{
					Players[i]->IsOnScreen = true;
				}
				SortedObjects[i] = Players[i];
				Players[i]->Init();
				Players[i]->InitPlayer();
			}
		}
		else
		{
			Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(APlayerCharacter::StaticClass());
			if (i % 3 == 0)
			{
				Players[i]->IsOnScreen = true;
			}
			SortedObjects[i] = Players[i];
			Players[i]->Init();
			Players[i]->InitPlayer();
		}
	}
	for (int i = 0; i < 400; i++)
	{
		Objects[i] = GetWorld()->SpawnActor<ABattleActor>(ABattleActor::StaticClass());
		SortedObjects[i + 6] = Objects[i];
	}

	if(GameInstance)
	{
		FActorSpawnParameters SpawnParms;
		SpawnParms.Owner = GetOwner();

		switch (GameInstance->FighterRunner)
		{
		case EFighterRunners::LocalPlay:
			FighterRunner = GetWorld()->SpawnActor<AFighterLocalRunner>(AFighterLocalRunner::StaticClass(),SpawnParms);
			break;
		case EFighterRunners::MULTIPLAYER:
			FighterRunner = GetWorld()->SpawnActor<AFighterLocalRunner>(AFighterMultiplayerRunner::StaticClass(),SpawnParms);
			break;
		case EFighterRunners::SYNCTEST:
			FighterRunner = GetWorld()->SpawnActor<AFighterLocalRunner>(AFighterSynctestRunner::StaticClass(),SpawnParms);
			break;
		default:
			FighterRunner = GetWorld()->SpawnActor<AFighterLocalRunner>(AFighterLocalRunner::StaticClass(),SpawnParms);
			break;
		}
	}
}

void AFighterGameState::Update(int Input1, int Input2)
{
	ParticleManager->UpdateParticles();
	SortObjects();
	FrameNumber++;
	SetFacing();
	HandlePushCollision();
	HandleHitCollision();
	CollisionView();
	SetWallCollision();
	SetScreenBounds();
	Players[0]->Inputs = Input1;
	Players[3]->Inputs = Input2;
	for (int i = 0; i < 6; i++)
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
}

void AFighterGameState::SaveGameState()
{	int BackupFrame = LocalFrame % MAX_ROLLBACK_FRAMES;
	RollbackData[BackupFrame].FrameNumber = FrameNumber;
	RollbackData[BackupFrame].ScreenPosition = CurrentScreenPos;
	RollbackData[BackupFrame].ActiveObjectCount = ActiveObjectCount;
	RollbackData[BackupFrame].Checksum = FrameNumber + CurrentScreenPos + ActiveObjectCount;
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
	CurrentScreenPos = RollbackData[CurrentRollbackFrame].ScreenPosition;
	ActiveObjectCount = RollbackData[CurrentRollbackFrame].ActiveObjectCount;
	for (int i = 0; i < 400; i++)
	{
		if (RollbackData[CurrentRollbackFrame].ObjActive[i])
		{
			Objects[i]->LoadForRollback(RollbackData[CurrentRollbackFrame].ObjBuffer[i]);
		}
		else
		{
			Objects[i]->IsActive = false;
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
	}
}

void AFighterGameState::UpdateCamera()
{
	if (CameraActor != nullptr)
	{
		if (!SequenceActor->SequencePlayer->IsPlaying())
		{
			float Distance = sqrt(abs((Players[0]->GetActorLocation() - Players[3]->GetActorLocation()).Y));
			Distance = FMath::Clamp(Distance,18, 25);
			float NewX = FMath::GetMappedRangeValueClamped(TRange<float>(0, 25), TRange<float>(0, 1080), Distance);
			FVector Average = (Players[0]->GetActorLocation() + Players[3]->GetActorLocation()) / 2;
			float NewY = FMath::Clamp(Average.Y,-630, 630);
			float NewZ = Average.Z + 175;
			CameraActor->SetActorLocation(FVector(-NewX, NewY, NewZ));
		}
		else
		{
			float Distance = sqrt(abs((Players[0]->GetActorLocation() - Players[3]->GetActorLocation()).Y));
			Distance = FMath::Clamp(Distance,18, 25);
			float NewX = FMath::GetMappedRangeValueClamped(TRange<float>(0, 25), TRange<float>(0, 1080), Distance);
			FVector Average = (Players[0]->GetActorLocation() + Players[3]->GetActorLocation()) / 2;
			float NewY = FMath::Clamp(Average.Y,-630, 630);
			float NewZ = Average.Z + 175;
			CameraActor->AddActorLocalOffset(FVector(-NewX, NewY, NewZ));
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
		}
	}
}

void AFighterGameState::PlayLevelSequence(ULevelSequence* Sequence)
{
	if (SequenceActor != nullptr)
	{
		SequenceActor->SetSequence(Sequence);
		SequenceActor->SequencePlayer->Stop();
		SequenceActor->SequencePlayer->Play();
	}
}

void AFighterGameState::AddBattleActor(UState* InState, int PosX, int PosY, bool FacingRight, APlayerCharacter* Parent)
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
			break;
		}
	}
}

void AFighterGameState::SetFacing()
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
						if(Players[i]->GetInternalValue(VAL_PosX) < Players[j]->GetInternalValue(VAL_PosX))
						{
							if(Players[i]->MiscFlags & MISC_FlipEnable)
							{
								Players[i]->SetFacing(true);
							}
							if(Players[j]->MiscFlags & MISC_FlipEnable)
							{
								Players[j]->SetFacing(false);
							}
						}
						else if(Players[i]->GetInternalValue(VAL_PosX) > Players[j]->GetInternalValue(VAL_PosX))
						{
							if(Players[i]->MiscFlags & MISC_FlipEnable)
							{
								Players[i]->SetFacing(false);
							}
							if(Players[j]->MiscFlags & MISC_FlipEnable)
							{
								Players[j]->SetFacing(true);
							}
						}
					}
				}
			}
		}
	}
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

void AFighterGameState::SetWallCollision()
{
	for (int i = 0; i < 6; i++)
	{
		if (Players[i] != nullptr)
		{
			if (Players[i]->IsOnScreen)
			{
				Players[i]->TouchingWall = true;
				if (Players[i]->GetInternalValue(VAL_PosX) > 1080000 + CurrentScreenPos)
				{
					Players[i]->SetPosX(1080000 + CurrentScreenPos);
				}
				else if (Players[i]->GetInternalValue(VAL_PosX) < -1080000 + CurrentScreenPos)
				{
					Players[i]->SetPosX(-1080000 + CurrentScreenPos);
				}
				else if (Players[i]->GetInternalValue(VAL_PosX) < 900000 + CurrentScreenPos || Players[i]->GetInternalValue(VAL_PosX) > -900000 + CurrentScreenPos)
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
						CurrentScreenPos = (Players[i]->GetInternalValue(VAL_PosX) + Players[j]->GetInternalValue(VAL_PosX)) / 2;
						if (CurrentScreenPos > 1080000)
						{
							CurrentScreenPos = 1080000;
						}
						else if (CurrentScreenPos < -1080000)
						{
							CurrentScreenPos = -1080000;
						}
					}
				}
			}
		}
	}
}
