// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CollisionBoxInternal.generated.h"

UENUM()
enum EBoxType
{
	Hurtbox,
	Hitbox,
};

/**
 * 
 */
USTRUCT()
struct FIGHTERENGINE_API FCollisionBoxInternal
{
	GENERATED_BODY()

	EBoxType Type;
	int PosX;
	int PosY;
	int SizeX;
	int SizeY;

	bool operator!=(const FCollisionBoxInternal& OtherBox)
	{
		return this->Type != OtherBox.Type || this->PosX != OtherBox.PosX || this->PosY != OtherBox.PosY
			|| this->SizeX != OtherBox.SizeX || this->SizeY != OtherBox.SizeY;
	}
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
	TEnumAsByte<EBoxType> Type;
	UPROPERTY(EditAnywhere)
	int PosX;
	UPROPERTY(EditAnywhere)
	int PosY;
	UPROPERTY(EditAnywhere)
	int SizeX;
	UPROPERTY(EditAnywhere)
	int SizeY;
};
