// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include <EditorFramework/AssetImportData.h>
#include "NightSkyBinary.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API UNightSkyBinary : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<uint8> Data;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Instanced, Category = Reimport)
	UAssetImportData* ImportData;
#endif
};

UCLASS()
class FIGHTERENGINE_API UNightSkyScript : public UNightSkyBinary
{
	GENERATED_BODY()
};
