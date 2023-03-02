// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LinkMeshData.generated.h"

USTRUCT(BlueprintType)
struct FMeshData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USkeletalMesh* Mesh;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UMaterialInterface*> Materials;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
    TSubclassOf<UAnimInstance> AnimBPClass;
};

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API ULinkMeshData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FMeshData> MeshDatas;
};
