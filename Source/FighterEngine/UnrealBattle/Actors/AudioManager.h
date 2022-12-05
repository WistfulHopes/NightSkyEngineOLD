// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioManager.generated.h"

class UAudioComponent;

UCLASS()
class FIGHTERENGINE_API AAudioManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAudioManager();

	UPROPERTY()
	UAudioComponent* CommonAudioPlayers[50];
	UPROPERTY()
	UAudioComponent* CharaAudioPlayers[50];
	UPROPERTY()
	UAudioComponent* CharaVoicePlayers[6];
	UPROPERTY()
	UAudioComponent* AnnouncerVoicePlayer;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PauseAllAudio();
};
