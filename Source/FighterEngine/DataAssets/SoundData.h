// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoundData.generated.h"

USTRUCT(BlueprintType)
struct FSoundDataStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* SoundWave;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDuration = 1.0f;
};

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API USoundData : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSoundDataStruct> SoundDatas;
};
