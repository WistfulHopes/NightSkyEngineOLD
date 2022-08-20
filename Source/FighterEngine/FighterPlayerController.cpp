// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterPlayerController.h"

#include "Bitflags.h"
#include "EngineUtils.h"
#include "FighterGameInstance.h"
#include "FighterGameState.h"
#include "NetworkPawn.h"
#include "Kismet/GameplayStatics.h"

void AFighterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	NetworkPawn = Cast<ANetworkPawn>(GetPawn());
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
		if (PlayerIndex == 0)
		{
			NetworkPawns[1]->ClientGetCharaData(Cast<UFighterGameInstance>(GetGameInstance())->PlayerList[0]);
		}
		else
		{
			NetworkPawns[0]->ServerGetCharaData(Cast<UFighterGameInstance>(GetGameInstance())->PlayerList[03]);
		}
	}
}
