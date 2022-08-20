// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AnimArray.generated.h"

//this is for storing animations to call

USTRUCT(BlueprintType)
struct FAnimData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimSequence* AnimSequence;
};

/**
 * 
 */
UCLASS(BlueprintType)
class FIGHTERENGINE_API UAnimArray : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FAnimData> AnimDatas;
};
