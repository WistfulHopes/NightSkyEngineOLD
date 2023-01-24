// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FighterEngine/Battle/State.h"
#include "StateDataAsset.generated.h"

//contains all states for the character

/**
 * 
 */
UCLASS(BlueprintType)
class FIGHTERENGINE_API UStateDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UState>> StateArray;
};
