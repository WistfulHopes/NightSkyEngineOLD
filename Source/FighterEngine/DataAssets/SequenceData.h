// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelSequence.h"
#include "Engine/DataAsset.h"
#include "SequenceData.generated.h"

USTRUCT(BlueprintType)
struct FSequenceStruct
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	ULevelSequence* Sequence;
};

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API USequenceData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FSequenceStruct> SequenceDatas;
};
