// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterMultiplayerRunner.h"
#include "Battle/Actors/FighterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "FighterEngine/UnrealBattle/Actors/UnrealFighterGameState.h"
#include "FighterEngine/Miscellaneous/RpcConnectionManager.h"
#include "UnrealBattle/Actors/FighterPlayerController.h"

// Sets default values
AFighterMultiplayerRunner::AFighterMultiplayerRunner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
}

// Called when the game starts or when spawned
void AFighterMultiplayerRunner::BeginPlay()
{
	Super::BeginPlay();
	GGPOSessionCallbacks cb = CreateCallbacks();
	connectionManager = new RpcConnectionManager(); //TODO delete on match end to prevent memleak
	GGPONet::ggpo_start_session(&ggpo, &cb, connectionManager,"", 2, sizeof(int));
	GGPONet::ggpo_set_disconnect_timeout(ggpo, 45000);
	GGPONet::ggpo_set_disconnect_notify_start(ggpo, 15000);
	for (int i = 0; i < 2; i++)
	{
		GGPOPlayerHandle handle;
		GGPOPlayer* player = new GGPOPlayer();
		player->type = GGPO_PLAYERTYPE_REMOTE;
		if(i == 0 && GetWorld()->GetNetMode()==ENetMode::NM_ListenServer)
		{
			player->type = GGPO_PLAYERTYPE_LOCAL;
			connectionManager->playerIndex = 1;	
		}
		if(i == 1 && GetWorld()->GetNetMode()==ENetMode::NM_Client)
		{
			player->type = GGPO_PLAYERTYPE_LOCAL;
			connectionManager->playerIndex = 0;
		}
		player->player_num = i + 1;
		player->connection_id = i;
		GGPONet::ggpo_add_player(ggpo, player, &handle);
		if(player->type == GGPO_PLAYERTYPE_LOCAL)
			GGPONet::ggpo_set_frame_delay(ggpo,handle,2);// TODO: Logic for framedelay (based on ping? or make user choose?)
		Players.Add(player);
		PlayerInputIndex.Add(-1);
		PlayerHandles.Add(handle);
	}
	GGPONet::ggpo_try_synchronize_local(ggpo);
}


GGPOSessionCallbacks AFighterMultiplayerRunner::CreateCallbacks()
{
	GGPOSessionCallbacks cb = {0};

	cb.begin_game = std::bind(&AFighterMultiplayerRunner::BeginGameCallback, this, std::placeholders::_1);
	cb.save_game_state = std::bind(&AFighterMultiplayerRunner::SaveGameStateCallback, this,
	                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
	                               std::placeholders::_4);
	cb.load_game_state = std::bind(&AFighterMultiplayerRunner::LoadGameStateCallback, this,
	                               std::placeholders::_1, std::placeholders::_2);
	cb.log_game_state = std::bind(&AFighterMultiplayerRunner::LogGameState, this,
	                              std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	cb.free_buffer = std::bind(&AFighterMultiplayerRunner::FreeBuffer, this, std::placeholders::_1);
	cb.advance_frame = std::bind(&AFighterMultiplayerRunner::AdvanceFrameCallback, this, std::placeholders::_1);
	cb.on_event = std::bind(&AFighterMultiplayerRunner::OnEventCallback, this, std::placeholders::_1);

	return cb;
}

bool AFighterMultiplayerRunner::BeginGameCallback(const char*)
{
	return true;
}

bool AFighterMultiplayerRunner::SaveGameStateCallback(unsigned char** buffer, int32* len, int32* checksum, int32)
{
	FighterGameState->InternalGameState.Get()->SaveGameState();
	RollbackData rollbackdata = FighterGameState->InternalGameState.Get()->StoredRollbackData;
	*len = sizeof(RollbackData);
	*buffer = new unsigned char[*len];
	FMemory::Memcpy(*buffer, &rollbackdata, *len);
	*checksum = fletcher32_checksum((short*)*buffer, *len / 2);
	return true;
}

bool AFighterMultiplayerRunner::LoadGameStateCallback(unsigned char* buffer, int32 len)
{
	int OldFrameNumber = FighterGameState->InternalGameState.Get()->StoredBattleState.FrameNumber;
	RollbackData* rollbackdata = &FighterGameState->InternalGameState.Get()->StoredRollbackData;
	FMemory::Memcpy(rollbackdata, buffer, len);
	FighterGameState->InternalGameState.Get()->LoadGameState();
	for (auto Object : FighterGameState->Objects)
		Object->PreUpdate();
	for (auto Player : FighterGameState->Players)
		Player->PreUpdate();
	
	for (ABattleActor* Object : FighterGameState->Objects)
	{
		if (Object->GetParent()->IsActive)
		{
			Object->FixObjectStateForRollback();
			Object->PreUpdate();
		}
	}
	for (APlayerCharacter* Player : FighterGameState->Players)
	{
		if (reinterpret_cast<PlayerCharacter*>(Player->GetParent())->IsOnScreen)
			Player->PreUpdate();
	}
	int FramesRolledBack = OldFrameNumber - FighterGameState->InternalGameState.Get()->StoredBattleState.FrameNumber;
	FighterGameState->RollbackStartAudio();
	FighterGameState->ParticleManager->RollbackParticles(FramesRolledBack);
	return true;
}

bool AFighterMultiplayerRunner::LogGameState(const char* filename, unsigned char* buffer, int len)
{
	FILE* fp = nullptr;
	FString savedDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());

	savedDir.Append(filename);
	fopen_s(&fp, TCHAR_TO_ANSI(*savedDir), "w");
	if (fp)
	{
		RollbackData* rollbackdata = (RollbackData*)malloc(sizeof(RollbackData));
		memcpy(rollbackdata, buffer, sizeof(RollbackData));
		fprintf(fp, "GameState:\n");
		BattleState InBattleState;
		memcpy(&InBattleState, rollbackdata->BattleStateBuffer, SIZEOF_BATTLESTATE);
		fprintf(fp, "\tFrameNumber:%d\n", InBattleState.FrameNumber);
		fprintf(fp, "\tActiveObjectCount:%d\n", rollbackdata->ActiveObjectCount);
		for (int i = 0; i < 400; i++)
		{
			if (rollbackdata->ObjActive[i])
			{
				BattleActor* CurBattleActor = new BattleActor;
				FMemory::Memcpy((char*)CurBattleActor + offsetof(BattleActor, ObjSync), rollbackdata->ObjBuffer[i], SIZEOF_BATTLEACTOR);
				CurBattleActor->LogForSyncTest(fp);
			}
		}
		for (int i = 400; i < 406; i++)
		{
			PlayerCharacter* CurPlayerCharacter = new PlayerCharacter;
			FMemory::Memcpy((char*)CurPlayerCharacter + offsetof(BattleActor, ObjSync), rollbackdata->ObjBuffer[i], SIZEOF_BATTLEACTOR);
			FMemory::Memcpy((char*)CurPlayerCharacter + offsetof(PlayerCharacter, PlayerSync), rollbackdata->CharBuffer[i - 400], SIZEOF_PLAYERCHARACTER);
			CurPlayerCharacter->LogForSyncTest(fp);
		}

		fprintf(fp,"RawRollbackData:\n");
		fprintf(fp, "\tObjBuffer:\n");
		for (int i = 0; i < 406; i++)
		{
			fprintf(fp, "Object %d\n", i);
			for (int x = 0; x < SIZEOF_BATTLEACTOR; x++)
			{
				fprintf(fp, " %x", rollbackdata->ObjBuffer[i][x]);	
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "\n");
		fprintf(fp, "\tObjActive:\n");
		for (int i = 0; i < 400; i++)
		{
			fprintf(fp, " %x", rollbackdata->ObjActive[i]);
		}
		fprintf(fp, "\n");
		fprintf(fp, "\tCharBuffer:\n");
		for (int i = 0; i < 6; i++)
		{
			fprintf(fp, "Character %d\n", i);
			for (int x = 0; x < SIZEOF_PLAYERCHARACTER; x++)
			{
				fprintf(fp, " %x", rollbackdata->CharBuffer[i][x]);	
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "\n");

		int checksum = fletcher32_checksum((short*)buffer, sizeof(RollbackData) / 2);
		fprintf(fp,"RawBuffer:\n");
		fprintf(fp, "\tFletcher32Checksum:%d\n", checksum);
		fprintf(fp, "\tBuffer:\n\t0: ");
		for (int i = 0; i < sizeof(RollbackData); i++)
		{
			fprintf(fp, " %x", buffer[i]);
			if((i%20)==0)
			{
				fprintf(fp, "\n\t%x: ",i);
			}
		}
		
		delete[] buffer;
		fclose(fp);
	}
	return true;
}

void AFighterMultiplayerRunner::FreeBuffer(void* buffer)
{
	delete[] buffer;
}

bool AFighterMultiplayerRunner::AdvanceFrameCallback(int flag)
{
	int inputs[2] = {0};
	int disconnect_flags;
	GGPONet::ggpo_synchronize_input(ggpo, (void*)inputs, sizeof(int) * 2, &disconnect_flags);
	for (ABattleActor* Object : FighterGameState->Objects)
	{
		if (Object->GetParent()->IsActive)
			Object->GetBoxes();
	}
	for (APlayerCharacter* Player : FighterGameState->Players)
	{
		if (reinterpret_cast<PlayerCharacter*>(Player->GetParent())->IsOnScreen)
			Player->GetBoxes();
	}
	FighterGameState->InternalGameState.Get()->Update(inputs[0], inputs[1]);
	FighterGameState->Update();
	GGPONet::ggpo_advance_frame(ggpo);
	return true;
}

bool AFighterMultiplayerRunner::OnEventCallback(GGPOEvent* info)
{
	int progress;
	switch (info->code)
	{
	case GGPO_EVENTCODE_CONNECTED_TO_PEER:
		UE_LOG(LogTemp, Warning, TEXT("GGPO_EVENTCODE_CONNECTED_TO_PEER"));
		break;
	case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
		UE_LOG(LogTemp, Warning, TEXT("GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER"));
	//synchronized = false;
		progress = 100 * info->u.synchronizing.count / info->u.synchronizing.total;
		break;
	case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
		UE_LOG(LogTemp, Warning, TEXT("GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER"));
	//synchronized = true;
		break;
	case GGPO_EVENTCODE_RUNNING:
		UE_LOG(LogTemp, Warning, TEXT("GGPO_EVENTCODE_RUNNING"));

		break;
	case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
		UE_LOG(LogTemp, Warning, TEXT("GGPO_EVENTCODE_CONNECTION_INTERRUPTED"));
	// connectionLost = true;
	// FightGameInstance->ErrorMessage = FString("Connection interrupted");
	// EndOnline(true);
		break;
	case GGPO_EVENTCODE_CONNECTION_RESUMED:
		//connectionLost = false;
		UE_LOG(LogTemp, Warning, TEXT("GGPO_EVENTCODE_CONNECTION_RESUMED"));

		break;
	case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
		UE_LOG(LogTemp, Warning, TEXT("GGPO_EVENTCODE_DISCONNECTED_FROM_PEER"));
		UGameplayStatics::OpenLevel(GetGameInstance(), FName(TEXT("Title")));
	//FightGameInstance->ErrorMessage = FString("Enemy disconnected");
	//EndOnline(true);
		break;
	case GGPO_EVENTCODE_TIMESYNC:
		UE_LOG(LogTemp, Warning, TEXT("GGPO_EVENTCODE_TIMESYNC"));
		//if(MultipliedFramesAhead>3)
		{
			MultipliedFramesAhead=info->u.timesync.frames_ahead*TimesyncMultiplier;
		}
		break;
	case GGPO_EVENTCODE_TIMESYNC_BEHIND:
		UE_LOG(LogTemp, Warning, TEXT("GGPO_EVENTCODE_TIMESYNC_BEHIND"));
		//MultipliedFramesBehind=info->u.timesync.frames_ahead*TimesyncMultiplier;
		break;
	}
	return true;
}


void AFighterMultiplayerRunner::GgpoUpdate()
{
	GGPOErrorCode result = GGPOErrorCode::GGPO_OK;
	int disconnect_flags;
	int inputs[2] = {0};
	for (int i = 0; i < 2; ++i)
	{
		if (Players[i]->type == GGPO_PLAYERTYPE_LOCAL)
		{
			int input = 0;
			if (PlayerInputIndex[i] == -1)
			{
				for (int index = 0; index < 4; index++)
				{
					if (AFighterPlayerController* PlayerController = Cast<AFighterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),index)))
					{
						PlayerController->UpdateInput();
					}
					input |= FighterGameState->InternalGameState.Get()->GetLocalInputs(index);
				}
			}
			else
			{
				if (AFighterPlayerController* PlayerController = Cast<AFighterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),i)))
				{
					PlayerController->UpdateInput();
				}
				input = FighterGameState->InternalGameState.Get()->GetLocalInputs(PlayerInputIndex[i]);
			}

			result = GGPONet::ggpo_add_local_input(ggpo, PlayerHandles[i], &input, sizeof(input));
		}
	}
	if (GGPO_SUCCEEDED(result))
	{
		result = GGPONet::ggpo_synchronize_input(ggpo, (void*)inputs, sizeof(int) * 2, &disconnect_flags);
		if (GGPO_SUCCEEDED(result))
		{
			for (ABattleActor* Object : FighterGameState->Objects)
			{
				if (Object->GetParent()->IsActive)
					Object->GetBoxes();
			}
			for (APlayerCharacter* Player : FighterGameState->Players)
			{
				if (reinterpret_cast<PlayerCharacter*>(Player->GetParent())->IsOnScreen)
					Player->GetBoxes();
			}
			FighterGameState->InternalGameState.Get()->Update(inputs[0], inputs[1]);
			FighterGameState->Update();
			GGPONet::ggpo_advance_frame(ggpo);
		}
	}
}

void AFighterMultiplayerRunner::Update(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	int accumulatorBreaker = 0;
	
	while (ElapsedTime >= ONE_FRAME && accumulatorBreaker < AccumulatorBreakerMax)
	{
		if(MultipliedFramesAhead>0)
		{
			int ahead = MultipliedFramesAhead;
			MultipliedFramesAhead--;
			if((ahead%TimesyncMultiplier)==0)
			{
				ElapsedTime=0;
				break;
			}
		}
		GgpoUpdate();
		ElapsedTime -= ONE_FRAME;
		accumulatorBreaker++;
		// if(MultipliedFramesBehind>0)
		// {
		// 	MultipliedFramesBehind--;
		// 	if(MultipliedFramesBehind%TimesyncMultiplier)
		// 	{
		// 		accumulatorBreaker=0;
		// 		ElapsedTime += ONE_FRAME;
		// 	}	
		// }
		//
	}
	GGPONet::ggpo_idle(ggpo,1);
}


//TODO refactor to a static libaray
int
AFighterMultiplayerRunner::fletcher32_checksum(short* data, size_t len)
{
	int sum1 = 0xffff, sum2 = 0xffff;

	while (len)
	{
		size_t tlen = len > 360 ? 360 : len;
		len -= tlen;
		do
		{
			sum1 += *data++;
			sum2 += sum1;
		}
		while (--tlen);
		sum1 = (sum1 & 0xffff) + (sum1 >> 16);
		sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	}

	/* Second reduction step to reduce sums to 16 bits */
	sum1 = (sum1 & 0xffff) + (sum1 >> 16);
	sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	return sum2 << 16 | sum1;
}
