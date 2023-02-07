// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NightSkyBinary.generated.h"

USTRUCT()
struct FScriptBlockOffsets
{
	GENERATED_BODY()
	
	uint32 OnEnterOffset = -1;
	uint32 OnUpdateOffset = -1;
	uint32 OnExitOffset = -1;
	uint32 OnLandingOffset = -1;
	uint32 OnHitOffset = -1;
	uint32 OnBlockOffset = -1;
	uint32 OnHitOrBlockOffset = -1;
	uint32 OnCounterHitOffset = -1;
	uint32 OnSuperFreezeOffset = -1;
	uint32 OnSuperFreezeEndOffset = -1;
};

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
};

UCLASS()
class FIGHTERENGINE_API UNightSkyScript : public UNightSkyBinary
{
	GENERATED_BODY()
};
