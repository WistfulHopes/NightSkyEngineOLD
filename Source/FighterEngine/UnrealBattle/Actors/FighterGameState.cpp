#include "FighterGameState.h"
#include "Battle/Actors/FighterGameState.h"

#include "DefaultLevelSequenceInstanceData.h"
#include "EngineUtils.h"
#include "FighterEngine/Miscellaneous/BattleUIActor.h"
#include "Net/UnrealNetwork.h"
#include "FighterEngine/Miscellaneous/FighterGameInstance.h"
#include "FighterPlayerController.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
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
	InternalGameState = TSharedPtr<FighterGameState>(new FighterGameState());
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		UGameplayStatics::CreatePlayer(GWorld);
		if (AFighterPlayerController* PlayerController = Cast<AFighterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0)))
		{
			InternalGameState.Get()->InputDevices[0] = static_cast<InputDevice*>(malloc(sizeof UnrealInputDevice));
			InternalGameState.Get()->InputDevices[0] = PlayerController->CurInputDevice.Get();
		}
		if (AFighterPlayerController* PlayerController = Cast<AFighterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),1)))
		{
			InternalGameState.Get()->InputDevices[1] = static_cast<InputDevice*>(malloc(sizeof UnrealInputDevice));
			InternalGameState.Get()->InputDevices[1] = PlayerController->CurInputDevice.Get();
		}
	}
	else if (GetWorld()->GetNetMode() == NM_ListenServer)
	{
		UGameplayStatics::CreatePlayer(GWorld);
		if (AFighterPlayerController* PlayerController = Cast<AFighterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0)))
		{
			InternalGameState.Get()->InputDevices[0] = static_cast<InputDevice*>(malloc(sizeof UnrealInputDevice));
			InternalGameState.Get()->InputDevices[0] = PlayerController->CurInputDevice.Get();
		}
	}
	else if (GetWorld()->GetNetMode() == NM_Client)
	{
		UGameplayStatics::CreatePlayer(GWorld);
		if (AFighterPlayerController* PlayerController = Cast<AFighterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0)))
		{
			InternalGameState.Get()->InputDevices[1] = static_cast<InputDevice*>(malloc(sizeof UnrealInputDevice));
			InternalGameState.Get()->InputDevices[1] = PlayerController->CurInputDevice.Get();
		}
	}
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

void AFighterGameState::Init()
{
	InternalGameState.Get()->Init();
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
				}
				else
				{
					Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(APlayerCharacter::StaticClass());
				}
			}
			else
			{
				Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(APlayerCharacter::StaticClass());
			}
		}
		else
		{
			Players[i] = GetWorld()->SpawnActor<APlayerCharacter>(APlayerCharacter::StaticClass());
		}
		Players[i]->GameState = this;
		Players[i]->SetParent(InternalGameState.Get()->Players[i]);
		Players[i]->InitStateMachine();
		Players[i]->Player = Players[i];
	}
	for (int i = 0; i < 400; i++)
	{
		Objects[i] = GetWorld()->SpawnActor<ABattleActor>(ABattleActor::StaticClass());
		Objects[i]->SetParent(InternalGameState.Get()->Objects[i]);
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

		InternalGameState.Get()->StoredBattleState.RoundFormat = (RoundFormat)GameInstance->RoundFormat;
		InternalGameState.Get()->StoredBattleState.RoundTimer = GameInstance->StartRoundTimer * 60;
	}
}

void AFighterGameState::Update()
{
	for (auto Object : Objects)
		Object->Update();
	for (auto Player : Players)
		Player->Update();
	ParticleManager->UpdateParticles();
	ManageAudio();
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
				BattleUIActor->Widget->P1Health[0] = float(reinterpret_cast<PlayerCharacter*>(Players[0]->GetParent())->CurrentHealth) / float(reinterpret_cast<PlayerCharacter*>(Players[0]->GetParent())->Health);
				BattleUIActor->Widget->P1Health[1] = float(reinterpret_cast<PlayerCharacter*>(Players[1]->GetParent())->CurrentHealth) / float(reinterpret_cast<PlayerCharacter*>(Players[1]->GetParent())->Health);
				BattleUIActor->Widget->P1Health[2] = float(reinterpret_cast<PlayerCharacter*>(Players[2]->GetParent())->CurrentHealth) / float(reinterpret_cast<PlayerCharacter*>(Players[2]->GetParent())->Health);
			}
			if (BattleUIActor->Widget->P2Health.Num() >= 3)
			{
				BattleUIActor->Widget->P2Health[0] = float(reinterpret_cast<PlayerCharacter*>(Players[3]->GetParent())->CurrentHealth) / float(reinterpret_cast<PlayerCharacter*>(Players[3]->GetParent())->Health);
				BattleUIActor->Widget->P2Health[1] = float(reinterpret_cast<PlayerCharacter*>(Players[4]->GetParent())->CurrentHealth) / float(reinterpret_cast<PlayerCharacter*>(Players[4]->GetParent())->Health);
				BattleUIActor->Widget->P2Health[2] = float(reinterpret_cast<PlayerCharacter*>(Players[5]->GetParent())->CurrentHealth) / float(reinterpret_cast<PlayerCharacter*>(Players[5]->GetParent())->Health);
			}
			BattleUIActor->Widget->P1RoundsWon = InternalGameState.Get()->StoredBattleState.P1RoundsWon;
			BattleUIActor->Widget->P2RoundsWon = InternalGameState.Get()->StoredBattleState.P2RoundsWon;
			BattleUIActor->Widget->Timer = ceil((float)InternalGameState.Get()->StoredBattleState.RoundTimer / 60);
			BattleUIActor->Widget->P1Meter = float(InternalGameState.Get()->StoredBattleState.Meter[0]) / 10000;
			BattleUIActor->Widget->P2Meter = float(InternalGameState.Get()->StoredBattleState.Meter[1]) / 10000;
			BattleUIActor->Widget->P1ComboCounter = reinterpret_cast<PlayerCharacter*>(Players[0]->GetParent())->ComboCounter;
			BattleUIActor->Widget->P2ComboCounter = reinterpret_cast<PlayerCharacter*>(Players[3]->GetParent())->ComboCounter;
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

void AFighterGameState::CollisionView()
{
	if (DisplayCollision)
	{
		for (int i = 0; i < 6; i++)
		{
			if (reinterpret_cast<PlayerCharacter*>(Players[i]->GetParent())->IsOnScreen)
			{
				Players[i]->CollisionView();
			}
		}
		for (int i = 0; i < 400; i++)
		{
			if (!Objects[i]->GetParent()->IsActive)
				continue;;
			Objects[i]->CollisionView();
		}
	}
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
		if (InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].Finished)
		{
			InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].SoundWave = (char*)InSoundWave;
			InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].StartingFrame = InternalGameState.Get()->StoredBattleState.FrameNumber;
			InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].MaxDuration = MaxDuration;
			InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].Finished = false;
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
		if (InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].Finished)
		{
			InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].SoundWave = (char*)InSoundWave;
			InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].StartingFrame = InternalGameState.Get()->StoredBattleState.FrameNumber;
			InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].MaxDuration = MaxDuration;
			InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].Finished = false;
			AudioManager->CharaAudioPlayers[i]->SetSound(InSoundWave);
			AudioManager->CharaAudioPlayers[i]->Play();
			return;
		}
	}
}

void AFighterGameState::PlayVoiceLine(USoundBase* InSoundWave, float MaxDuration, int Player)
{
	InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[Player].SoundWave = (char*)InSoundWave;
	InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[Player].StartingFrame = InternalGameState.Get()->StoredBattleState.FrameNumber;
	InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[Player].MaxDuration = MaxDuration;
	InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[Player].Finished = false;
	AudioManager->CharaVoicePlayers[Player]->SetSound(InSoundWave);
	AudioManager->CharaVoicePlayers[Player]->Play();
}

void AFighterGameState::ManageAudio()
{
	for (int i = 0; i < CommonAudioChannelCount; i++)
	{
		int CurrentAudioTime = InternalGameState.Get()->StoredBattleState.FrameNumber - InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].StartingFrame;
		if (!InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].Finished && static_cast<int>(InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].MaxDuration * 60) < CurrentAudioTime)
		{
			InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].Finished = true;
			AudioManager->CommonAudioPlayers[i]->Stop();
			AudioManager->CommonAudioPlayers[i]->SetSound(nullptr);
		}
	}
	for (int i = 0; i < CharaAudioChannelCount; i++)
	{
		int CurrentAudioTime = InternalGameState.Get()->StoredBattleState.FrameNumber - InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].StartingFrame;
		if (!InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].Finished && static_cast<int>(InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].MaxDuration * 60) < CurrentAudioTime)
		{
			InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].Finished = true;
			AudioManager->CharaAudioPlayers[i]->Stop();
			AudioManager->CharaAudioPlayers[i]->SetSound(nullptr);
		}
	}
	for (int i = 0; i < CharaVoiceChannelCount; i++)
	{
		int CurrentAudioTime = InternalGameState.Get()->StoredBattleState.FrameNumber - InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[i].StartingFrame;
		if (!InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[i].Finished && static_cast<int>(InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[i].MaxDuration * 60) < CurrentAudioTime)
		{
			InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[i].Finished = true;
			AudioManager->CharaVoicePlayers[i]->Stop();
			AudioManager->CharaVoicePlayers[i]->SetSound(nullptr);
		}
	}
	int CurrentAudioTime = InternalGameState.Get()->StoredBattleState.FrameNumber - InternalGameState.Get()->StoredBattleState.AnnouncerVoiceChannel.StartingFrame;
	if (!InternalGameState.Get()->StoredBattleState.AnnouncerVoiceChannel.Finished && static_cast<int>(InternalGameState.Get()->StoredBattleState.AnnouncerVoiceChannel.MaxDuration * 60) < CurrentAudioTime)
	{
		InternalGameState.Get()->StoredBattleState.AnnouncerVoiceChannel.Finished = true;
		AudioManager->AnnouncerVoicePlayer->Stop();
		AudioManager->AnnouncerVoicePlayer->SetSound(nullptr);
	}
}

void AFighterGameState::RollbackStartAudio()
{
	for (int i = 0; i < CommonAudioChannelCount; i++)
	{
		if (InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].SoundWave != (char*)AudioManager->CommonAudioPlayers[i]->GetSound())
		{
			AudioManager->CommonAudioPlayers[i]->Stop();
			AudioManager->CommonAudioPlayers[i]->SetSound((USoundBase*)InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].SoundWave);
			float CurrentAudioTime = float(InternalGameState.Get()->StoredBattleState.FrameNumber - InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].StartingFrame) / 60.f;
			if (!InternalGameState.Get()->StoredBattleState.CommonAudioChannels[i].Finished && !AudioManager->CommonAudioPlayers[i]->IsPlaying())
			{
				AudioManager->CommonAudioPlayers[i]->SetFloatParameter(FName(TEXT("Start Time")), CurrentAudioTime);
				AudioManager->CommonAudioPlayers[i]->Play(CurrentAudioTime);
			}
		}
	}
	for (int i = 0; i < CharaAudioChannelCount; i++)
	{
		if (InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].SoundWave != (char*)AudioManager->CharaAudioPlayers[i]->GetSound())
		{
			AudioManager->CharaAudioPlayers[i]->Stop();
			AudioManager->CharaAudioPlayers[i]->SetSound((USoundBase*)InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].SoundWave);
			float CurrentAudioTime = float(InternalGameState.Get()->StoredBattleState.FrameNumber - InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].StartingFrame) / 60.f;
			if (!InternalGameState.Get()->StoredBattleState.CharaAudioChannels[i].Finished && !AudioManager->CharaAudioPlayers[i]->IsPlaying())
			{
				AudioManager->CharaAudioPlayers[i]->SetFloatParameter(FName(TEXT("Start Time")), CurrentAudioTime);
				AudioManager->CharaAudioPlayers[i]->Play(CurrentAudioTime);
			}
		}
	}
	for (int i = 0; i < CharaVoiceChannelCount; i++)
	{
		if (InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[i].SoundWave != (char*)AudioManager->CharaVoicePlayers[i]->GetSound())
		{
			AudioManager->CharaVoicePlayers[i]->Stop();
			AudioManager->CharaVoicePlayers[i]->SetSound((USoundBase*)InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[i].SoundWave);
			float CurrentAudioTime = float(InternalGameState.Get()->StoredBattleState.FrameNumber - InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[i].StartingFrame) / 60.f;
			if (!InternalGameState.Get()->StoredBattleState.CharaVoiceChannels[i].Finished && !AudioManager->CharaVoicePlayers[i]->IsPlaying())
			{
				AudioManager->CharaVoicePlayers[i]->SetFloatParameter(FName(TEXT("Start Time")), CurrentAudioTime);
				AudioManager->CharaVoicePlayers[i]->Play(CurrentAudioTime);
			}
		}
	}
	if (InternalGameState.Get()->StoredBattleState.AnnouncerVoiceChannel.SoundWave != (char*)AudioManager->AnnouncerVoicePlayer->GetSound())
	{
		AudioManager->AnnouncerVoicePlayer->Stop();
		AudioManager->AnnouncerVoicePlayer->SetSound((USoundBase*)InternalGameState.Get()->StoredBattleState.AnnouncerVoiceChannel.SoundWave);
		float CurrentAudioTime = float(InternalGameState.Get()->StoredBattleState.FrameNumber - InternalGameState.Get()->StoredBattleState.AnnouncerVoiceChannel.StartingFrame) / 60.f;
		if (!InternalGameState.Get()->StoredBattleState.AnnouncerVoiceChannel.Finished && !AudioManager->AnnouncerVoicePlayer->IsPlaying())
		{
			AudioManager->AnnouncerVoicePlayer->SetFloatParameter(FName(TEXT("Start Time")), CurrentAudioTime);
			AudioManager->AnnouncerVoicePlayer->Play(CurrentAudioTime);
		}
	}
}
