// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPawn.h"

#include "FighterGameInstance.h"
#include "RpcConnectionManager.h"
#include "Battle/Actors/FighterGameState.h"
#include "FighterEngine/UnrealBattle/Actors/FighterGameState.h"
#include "Net/UnrealNetwork.h"
#include "FighterEngine/FighterRunners/FighterMultiplayerRunner.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANetworkPawn::ANetworkPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ANetworkPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ANetworkPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkPawn, CharaDataReceived)
}

void ANetworkPawn::SendGgpoToClient_Implementation(const TArray<int8> &GgpoMessage)
{
	 if(FighterMultiplayerRunner)
	 	FighterMultiplayerRunner->connectionManager->receiveSchedule.AddTail(GgpoMessage);
}

void ANetworkPawn::SendGgpoToServer_Implementation(const TArray<int8> &GgpoMessage)
{
	 if(FighterMultiplayerRunner)
	 	FighterMultiplayerRunner->connectionManager->receiveSchedule.AddTail(GgpoMessage);
}

void ANetworkPawn::ClientGetCharaData_Implementation(TSubclassOf<APlayerCharacter> CharaClass, ERoundFormat InRoundFormat, int InRoundTimer)
{
	UFighterGameInstance* GameInstance = Cast<UFighterGameInstance>(GetGameInstance());
	GameInstance->PlayerList[0] = CharaClass;
	GameInstance->RoundFormat = InRoundFormat;
	GameInstance->StartRoundTimer = InRoundTimer;
	CharaDataReceived = true;
}

void ANetworkPawn::ServerGetCharaData_Implementation(TSubclassOf<APlayerCharacter> CharaClass)
{
	UFighterGameInstance* GameInstance = Cast<UFighterGameInstance>(GetGameInstance());
	GameInstance->PlayerList[3] = CharaClass;
	CharaDataReceived = true;
}

void ANetworkPawn::SendToClient_Implementation(const int32 InInputs)
{
	if (AFighterGameState* GameState = Cast<AFighterGameState>(GetWorld()->GetGameState()))
	{
		if (GameState != nullptr)
		{
			GameState->InternalGameState.Get()->UpdateRemoteInput(InInputs);
		}
	}
}

void ANetworkPawn::SendToServer_Implementation(const int32 InInputs)
{
	if (AFighterGameState* GameState = Cast<AFighterGameState>(GetWorld()->GetGameState()))
	{
		if (GameState != nullptr)
		{
			GameState->InternalGameState.Get()->UpdateRemoteInput(InInputs);
		}
	}
}

