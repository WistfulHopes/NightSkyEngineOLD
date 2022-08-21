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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool CharaDataReceived;
	
	UFUNCTION( Server, Reliable )
	void SendToServer(const TArray<int32> &InInputs, int32 InFrame, int32 InFrameAdvantage);
	UFUNCTION( Client, Reliable )
	void SendToClient(const TArray<int32> &InInputs, int32 InFrame, int32 InFrameAdvantage);
	UFUNCTION( Server, Reliable )
	void ServerChecksumCheck(uint32 Checksum, int32 InFrame);
	UFUNCTION( Client, Reliable )
	void ClientChecksumCheck(uint32 Checksum, int32 InFrame);
	UFUNCTION( Server, Reliable )
	void ServerGetCharaData(TSubclassOf<APlayerCharacter> CharaClass);
	UFUNCTION( Client, Reliable )
	void ClientGetCharaData(TSubclassOf<APlayerCharacter> CharaClass);

	class AFighterMultiplayerRunner* FighterMultiplayerRunner=nullptr;
	UFUNCTION( Server, Unreliable )
	void SendGgpoToServer(const TArray<int32> &GgpoMessage);
	UFUNCTION( Client, Unreliable )
	void SendGgpoToClient(const TArray<int32> &GgpoMessage);
};
