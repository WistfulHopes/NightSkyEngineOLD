// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Subroutine.generated.h"

class APlayerCharacter;
class ABattleActor;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FIGHTERENGINE_API USubroutine : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	ABattleActor* Parent;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Name;

	UFUNCTION(BlueprintNativeEvent)
	void OnCall(); //executes on call. write in bp
};
