// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StageDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FStageData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString StageName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* StageTexture;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString StageURL;
};

/**
 * 
 */
UCLASS(BlueprintType)
class FIGHTERENGINE_API UStageDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FStageData> StageDatas;
};
