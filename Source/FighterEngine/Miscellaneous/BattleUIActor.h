// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FighterBattleWidget.h"
#include "GameFramework/Actor.h"
#include "BattleUIActor.generated.h"

UCLASS()
class FIGHTERENGINE_API ABattleUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattleUIActor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UFighterBattleWidget* Widget;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
