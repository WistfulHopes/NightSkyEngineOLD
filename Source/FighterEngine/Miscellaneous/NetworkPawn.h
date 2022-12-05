// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NetworkPawn.generated.h"

class APlayerCharacter;

UCLASS()
class FIGHTERENGINE_API ANetworkPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANetworkPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool CharaDataReceived;
	
	UFUNCTION( Server, Reliable )
	void SendToServer(const int32 InInputs);
	UFUNCTION( Client, Reliable )
	void SendToClient(const int32 InInputs);
	UFUNCTION( Server, Reliable )
	void ServerGetCharaData(TSubclassOf<APlayerCharacter> CharaClass);
	UFUNCTION( Client, Reliable )
	void ClientGetCharaData(TSubclassOf<APlayerCharacter> CharaClass, enum ERoundFormat InRoundFormat, int InRoundTimer);

	UPROPERTY()
	class AFighterMultiplayerRunner* FighterMultiplayerRunner=nullptr;
	UFUNCTION( Server, Unreliable )
	void SendGgpoToServer(const TArray<int8> &GgpoMessage);
	UFUNCTION( Client, Unreliable )
	void SendGgpoToClient(const TArray<int8> &GgpoMessage);
};
