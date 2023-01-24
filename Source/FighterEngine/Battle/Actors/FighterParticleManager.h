// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "FighterParticleManager.generated.h"

UCLASS()
class FIGHTERENGINE_API AFighterParticleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFighterParticleManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateParticles();

	UFUNCTION(BlueprintImplementableEvent)
	void RollbackParticles(int RollbackFrames);

	UPROPERTY(BlueprintReadWrite)
	TArray<UNiagaraComponent*> NiagaraComponents;
};
