// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Battle/CollisionBoxInternal.h"
#include "Engine/DataAsset.h"
#include "CollisionData.generated.h"

USTRUCT(BlueprintType)
struct FCollisionFrame
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FString Name;
	UPROPERTY(EditAnywhere, Instanced)
	TArray<UCollisionBox*> CollisionBoxes;
};

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API UCollisionData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FCollisionFrame> CollisionFrames;
};