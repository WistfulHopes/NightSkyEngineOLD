// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterPlayerController.h"

#include "FighterEngine/Battle/Bitflags.h"
#include "EngineUtils.h"
#include "FighterEngine/Miscellaneous/FighterGameInstance.h"
#include "FighterGameState.h"
#include "FighterEngine/Miscellaneous/NetworkPawn.h"
#include "FighterEngine/Miscellaneous/RpcConnectionManager.h"
#include "FighterRunners/FighterMultiplayerRunner.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "FighterEngine/FighterGameModeBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"

void AFighterPlayerController::OnNetCleanup(UNetConnection* Connection)
{
	if (GetLocalRole() == ROLE_Authority && PlayerState != NULL)
	{
		AFighterGameModeBase *GameMode = Cast<AFighterGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->PreLogout(this);
		}
	}

	Super::OnNetCleanup(Connection);
}

void AFighterPlayerController::TrySettingFighterCameraToViewport()
{
	if(CurrentView!=nullptr) return;
	for (TActorIterator<ACameraActor> It(GetWorld()); It;++It)
	{
		if(It->GetName().Contains("FighterCamera"))
		{
			CurrentView = (*It);
			SetViewTarget(CurrentView);
		}
	}
}

void AFighterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	NetworkPawn = Cast<ANetworkPawn>(GetPawn());
	TrySettingFighterCameraToViewport();
}

void AFighterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TrySettingFighterCameraToViewport();
	if (NetworkPawn != nullptr)
	{
		const int PlayerIndex = Cast<UFighterGameInstance>(GetGameInstance())->PlayerIndex;
		TArray<ANetworkPawn*> NetworkPawns;
		for (TActorIterator<ANetworkPawn> It(GetWorld()); It; ++It)
		{
			NetworkPawns.Add(*It);
		}
		if (NetworkPawns.Num() > 1)
		{
			if (PlayerIndex == 0)
			{
				SendGgpo(NetworkPawns[1], true);
			}
			else
			{
				SendGgpo(NetworkPawns[0], false);
			}
		}
	}
}

void AFighterPlayerController::SetupInputComponent()
{
	if (GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (!InputMapping.IsNull())
			{
				InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 0);
			}
		}
	}
	
	if (InputComponent == NULL)
	{
		InputComponent = NewObject<UEnhancedInputComponent>(this, UInputSettings::GetDefaultInputComponentClass(), TEXT("PC_InputComponent0"));
		InputComponent->RegisterComponent();
	}

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);

	
	for (auto Mapping : InputMapping.Get()->GetMappings())
	{
		FString ActionName = Mapping.Action.Get()->ActionDescription.ToString();
		if (ActionName == "Press Up")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressUp);
		}
		else if (ActionName == "Release Up")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseUp);
		}
		else if (ActionName == "Press Down")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressDown);
		}
		else if (ActionName == "Release Down")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseDown);
		}
		else if (ActionName == "Press Left")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressLeft);
		}
		else if (ActionName == "Release Left")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseLeft);
		}
		else if (ActionName == "Press Right")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressRight);
		}
		else if (ActionName == "Release Right")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseRight);
		}
		else if (ActionName == "Press Light Attack")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressL);
		}
		else if (ActionName == "Release Light Attack")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseL);
		}
		else if (ActionName == "Press Medium Attack")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressM);
		}
		else if (ActionName == "Release Medium Attack")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseM);
		}
		else if (ActionName == "Press Heavy Attack")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressH);
		}
		else if (ActionName == "Release Heavy Attack")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseH);
		}
		else if (ActionName == "Press Special Attack")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressS);
		}
		else if (ActionName == "Release Special Attack")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseS);
		}
		else if (ActionName == "Press Assist 1")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressA1);
		}
		else if (ActionName == "Release Assist 1")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseA1);
		}
		else if (ActionName == "Press Assist 2")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressA2);
		}
		else if (ActionName == "Release Assist 2")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseA2);
		}
		else if (ActionName == "Press Dash")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::PressDash);
		}
		else if (ActionName == "Release Dash")
		{
			Input->BindAction(Mapping.Action.Get(), ETriggerEvent::Triggered, this, &AFighterPlayerController::ReleaseDash);
		}
	}
}

void AFighterPlayerController::PressUp()
{
	Inputs |= InputUp;
}

void AFighterPlayerController::ReleaseUp()
{
	Inputs = Inputs & ~InputUp;
}

void AFighterPlayerController::PressDown()
{
	Inputs |= InputDown;
}

void AFighterPlayerController::ReleaseDown()
{
	Inputs = Inputs & ~InputDown;
}

void AFighterPlayerController::PressLeft()
{
	Inputs |= InputLeft;
}

void AFighterPlayerController::ReleaseLeft()
{
	Inputs = Inputs & ~InputLeft;
}

void AFighterPlayerController::PressRight()
{
	Inputs |= InputRight;
}

void AFighterPlayerController::ReleaseRight()
{
	Inputs = Inputs & ~InputRight;
}

void AFighterPlayerController::PressL()
{
	Inputs |= InputL;
}

void AFighterPlayerController::ReleaseL()
{
	Inputs = Inputs & ~InputL;
}

void AFighterPlayerController::PressM()
{
	Inputs |= InputM;
}

void AFighterPlayerController::ReleaseM()
{
	Inputs = Inputs & ~InputM;
}

void AFighterPlayerController::PressH()
{
	Inputs |= InputH;
}

void AFighterPlayerController::ReleaseH()
{
	Inputs = Inputs & ~InputH;
}

void AFighterPlayerController::PressS()
{
	Inputs |= InputS;
}

void AFighterPlayerController::ReleaseS()
{
	Inputs = Inputs & ~InputS;
}

void AFighterPlayerController::PressA1()
{
	Inputs |= InputA1;	
}

void AFighterPlayerController::ReleaseA1()
{
	Inputs = Inputs & ~InputA1;
}

void AFighterPlayerController::PressA2()
{
	Inputs |= InputA2;	
}

void AFighterPlayerController::ReleaseA2()
{
	Inputs = Inputs & ~InputA2;
}

void AFighterPlayerController::PressDash()
{
	Inputs |= InputDash;
}

void AFighterPlayerController::ReleaseDash()
{
	Inputs = Inputs & ~InputDash;
}

void AFighterPlayerController::UpdateInput(int Input[], int32 InFrame, int32 InFrameAdvantage)
{
	const int PlayerIndex = Cast<UFighterGameInstance>(GetGameInstance())->PlayerIndex;
	TArray<ANetworkPawn*> NetworkPawns;
	for (TActorIterator<ANetworkPawn> It(GetWorld()); It; ++It)
	{
		NetworkPawns.Add(*It);
	}
	TArray<int32> SendInputs;
	for (int i = 0; i < MAX_ROLLBACK_FRAMES; i++)
	{
		SendInputs.Add(Input[i]);
	}
	if (NetworkPawns.Num() > 1)
	{
		if (PlayerIndex == 0)
		{
			for (int i = 0; i < MAX_ROLLBACK_FRAMES; i++)
			{
				NetworkPawns[1]->SendToClient(SendInputs, InFrame, InFrameAdvantage);
			}
		}
		else
		{
			for (int i = 0; i < MAX_ROLLBACK_FRAMES; i++)
			{
				NetworkPawns[0]->SendToServer(SendInputs, InFrame, InFrameAdvantage);
			}
		}
	}
}

void AFighterPlayerController::CheckForDesyncs(uint32 Checksum, int32 InFrame)
{
	const int PlayerIndex = Cast<UFighterGameInstance>(GetGameInstance())->PlayerIndex;
	TArray<ANetworkPawn*> NetworkPawns;
	for (TActorIterator<ANetworkPawn> It(GetWorld()); It; ++It)
	{
		NetworkPawns.Add(*It);
	}
	if (NetworkPawns.Num() > 1)
	{
		if (PlayerIndex == 0)
		{
			NetworkPawns[1]->ClientChecksumCheck(Checksum, InFrame);
		}
		else
		{
			NetworkPawns[0]->ServerChecksumCheck(Checksum, InFrame);
		}
	}
}

void AFighterPlayerController::SendGgpo(ANetworkPawn* InNetworkPawn, bool Client)
{
	if(InNetworkPawn->FighterMultiplayerRunner==nullptr)//TODO: CHECK IF MULTIPLAYERRUNNER IS SPAWNED BEFORE THIS, IF SO DO THIS IN BEGINPLAY
		{
		TArray<AActor*> FoundFighterGameStates;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFighterMultiplayerRunner::StaticClass(), FoundFighterGameStates);
		if(FoundFighterGameStates.Num()>0){
			InNetworkPawn->FighterMultiplayerRunner = Cast<AFighterMultiplayerRunner>(FoundFighterGameStates[0]);
		}
		}

	if (InNetworkPawn->FighterMultiplayerRunner && InNetworkPawn->FighterMultiplayerRunner->connectionManager)
	{
		while(InNetworkPawn->FighterMultiplayerRunner->connectionManager->sendSchedule.Num()>0)
		{
			auto SendVal = InNetworkPawn->FighterMultiplayerRunner->connectionManager->sendSchedule.GetTail();
			if(Client)
			{
				InNetworkPawn->SendGgpoToClient(SendVal->GetValue());
			}
			else
			{
				InNetworkPawn->SendGgpoToServer(SendVal->GetValue());
			}
			InNetworkPawn->FighterMultiplayerRunner->connectionManager->sendSchedule.Empty();
			//InNetworkPawn->FighterMultiplayerRunner->connectionManager->sendSchedule.RemoveNode(SendVal);
		}
	}
}

void AFighterPlayerController::SendCharaData()
{
	int PlayerIndex = 0;
	if (GetWorld()->GetNetMode() == NM_Client)
		PlayerIndex = Cast<UFighterGameInstance>(GetGameInstance())->PlayerIndex = 1;
	TArray<ANetworkPawn*> NetworkPawns;
	for (TActorIterator<ANetworkPawn> It(GetWorld()); It; ++It)
	{
		NetworkPawns.Add(*It);
	}
	if (NetworkPawns.Num() > 1)
	{
		UFighterGameInstance* GameInstance = Cast<UFighterGameInstance>(GetGameInstance());
		if (PlayerIndex == 0)
		{
			NetworkPawns[1]->ClientGetCharaData(GameInstance->PlayerList[0], GameInstance->RoundFormat, GameInstance->StartRoundTimer);
		}
		else
		{
			NetworkPawns[0]->ServerGetCharaData(GameInstance->PlayerList[03]);
		}
	}
}
