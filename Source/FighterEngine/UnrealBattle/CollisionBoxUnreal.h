// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CollisionBoxUnreal.generated.h"

UENUM()
enum class EBoxType : uint8
{
	Hurtbox,
	Hitbox,
};

/**
 * 
 */
UCLASS(BlueprintType, EditInlineNew)
class FIGHTERENGINE_API UCollisionBox : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	EBoxType Type;
	UPROPERTY(EditAnywhere)
	int PosX;
	UPROPERTY(EditAnywhere)
	int PosY;
	UPROPERTY(EditAnywhere)
	int SizeX;
	UPROPERTY(EditAnywhere)
	int SizeY;
};
