// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraFunctionLibrary.h"
#include "ParticleData.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FParticleStruct
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UNiagaraSystem* ParticleSystem;
};

/**
 * 
 */
UCLASS(BlueprintType)
class FIGHTERENGINE_API UParticleData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FParticleStruct> ParticleDatas;
};
