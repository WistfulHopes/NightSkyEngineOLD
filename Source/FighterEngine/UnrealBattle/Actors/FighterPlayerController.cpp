// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterPlayerController.h"

#include "Battle/Bitflags.h"
#include "EngineUtils.h"
#include "FighterEngine/Miscellaneous/FighterGameInstance.h"
#include "FighterGameState.h"
#include "FighterEngine/Miscellaneous/NetworkPawn.h"
#include "FighterEngine/Miscellaneous/RpcConnectionManager.h"
#include "FighterRunners/FighterMultiplayerRunner.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

AFighterPlayerController::AFighterPlayerController()
{
	CurInputDevice = TSharedPtr<UnrealInputDevice>(new UnrealInputDevice);
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
	Super::SetupInputComponent();

	InputComponent->BindAction("Up", IE_Pressed, this, &AFighterPlayerController::PressUp);
	InputComponent->BindAction("Up", IE_Released, this, &AFighterPlayerController::ReleaseUp);
	InputComponent->BindAction("Down", IE_Pressed, this, &AFighterPlayerController::PressDown);
	InputComponent->BindAction("Down", IE_Released, this, &AFighterPlayerController::ReleaseDown);
	InputComponent->BindAction("Left", IE_Pressed, this, &AFighterPlayerController::PressLeft);
	InputComponent->BindAction("Left", IE_Released, this, &AFighterPlayerController::ReleaseLeft);
	InputComponent->BindAction("Right", IE_Pressed, this, &AFighterPlayerController::PressRight);
	InputComponent->BindAction("Right", IE_Released, this, &AFighterPlayerController::ReleaseRight);
	InputComponent->BindAction("LightAttack", IE_Pressed, this, &AFighterPlayerController::PressL);
	InputComponent->BindAction("LightAttack", IE_Released, this, &AFighterPlayerController::ReleaseL);
	InputComponent->BindAction("MediumAttack", IE_Pressed, this, &AFighterPlayerController::PressM);
	InputComponent->BindAction("MediumAttack", IE_Released, this, &AFighterPlayerController::ReleaseM);
	InputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AFighterPlayerController::PressH);
	InputComponent->BindAction("HeavyAttack", IE_Released, this, &AFighterPlayerController::ReleaseH);
	InputComponent->BindAction("SpecialAttack", IE_Pressed, this, &AFighterPlayerController::PressS);
	InputComponent->BindAction("SpecialAttack", IE_Released, this, &AFighterPlayerController::ReleaseS);
}

void AFighterPlayerController::PressUp()
{
	Inputs |= InputFlags::InputUp;
}

void AFighterPlayerController::ReleaseUp()
{
	Inputs = Inputs & ~InputFlags::InputUp;
}

void AFighterPlayerController::PressDown()
{
	Inputs |= InputFlags::InputDown;
}

void AFighterPlayerController::ReleaseDown()
{
	Inputs = Inputs & ~InputFlags::InputDown;
}

void AFighterPlayerController::PressLeft()
{
	Inputs |= InputFlags::InputLeft;
}

void AFighterPlayerController::ReleaseLeft()
{
	Inputs = Inputs & ~InputFlags::InputLeft;
}

void AFighterPlayerController::PressRight()
{
	Inputs |= InputFlags::InputRight;
}

void AFighterPlayerController::ReleaseRight()
{
	Inputs = Inputs & ~InputFlags::InputRight;
}

void AFighterPlayerController::PressL()
{
	Inputs |= InputFlags::InputL;
}

void AFighterPlayerController::ReleaseL()
{
	Inputs = Inputs & ~InputFlags::InputL;
}

void AFighterPlayerController::PressM()
{
	Inputs |= InputFlags::InputM;
}

void AFighterPlayerController::ReleaseM()
{
	Inputs = Inputs & ~InputFlags::InputM;
}

void AFighterPlayerController::PressH()
{
	Inputs |= InputFlags::InputH;
}

void AFighterPlayerController::ReleaseH()
{
	Inputs = Inputs & ~InputFlags::InputH;
}

void AFighterPlayerController::PressS()
{
	Inputs |= InputFlags::InputS;
}

void AFighterPlayerController::ReleaseS()
{
	Inputs = Inputs & ~InputFlags::InputS;
}

void AFighterPlayerController::UpdateInput()
{
	const int PlayerIndex = Cast<UFighterGameInstance>(GetGameInstance())->PlayerIndex;
	TArray<ANetworkPawn*> NetworkPawns;
	for (TActorIterator<ANetworkPawn> It(GetWorld()); It; ++It)
	{
		NetworkPawns.Add(*It);
	}
	CurInputDevice.Get()->Inputs = Inputs;
	if (NetworkPawns.Num() > 1)
	{
		if (PlayerIndex == 0)
		{
			NetworkPawns[1]->SendToClient(Inputs);
		}
		else
		{
			NetworkPawns[0]->SendToServer(Inputs);
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
