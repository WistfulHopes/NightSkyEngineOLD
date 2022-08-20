// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Engine/DataAsset.h"
#include "CharaSelectData.generated.h"

USTRUCT(BlueprintType)
struct FCharaData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString CharaName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* CharaTexture;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<APlayerCharacter> PlayerClass;
};

/**
 * 
 */
UCLASS(BlueprintType)
class FIGHTERENGINE_API UCharaSelectData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FCharaData> CharaDatas;
};
