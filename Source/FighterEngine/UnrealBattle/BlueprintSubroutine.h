// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/Subroutine.h"
#include "BlueprintSubroutine.generated.h"

class APlayerCharacter;
class ABattleActor;

class BlueprintSubroutine : public Subroutine
{
	class USubroutine* Owner;
public:
	BlueprintSubroutine(USubroutine* InSubroutine);	
	virtual void OnCall() override;
};

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
	TSharedPtr<BlueprintSubroutine> ParentSubroutine;

	UFUNCTION(BlueprintImplementableEvent)
	void OnCall(); //executes on call. write in bp
};
