// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "State.h"
#include "Bitflags.h"
#include "StateMachine.generated.h"

class APlayerCharacter; //forward declaration

/**
 * 
 */
UCLASS(BlueprintType)
class FIGHTERENGINE_API UStateMachine : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY()
	UState* CurrentState;

public:
	UPROPERTY()
	TArray<UState*> States;

	UPROPERTY()
	TArray<FString> StateNames;

	UPROPERTY()
	APlayerCharacter* Parent;

	UFUNCTION(BlueprintCallable)
	void AddState(const FString Name, UState* Config)
	{
		Config->Parent = Parent;
		States.Add(Config);
		StateNames.Add(Name);
		if (CurrentState == nullptr)
		{
			CurrentState = Config;
			CurrentState->OnEnter();
		}
	}

	bool IsCurrentState(const FString Name)
	{
		return CurrentState->Name == Name;
	}

	FString GetStateName(int Index);
	int GetStateIndex(FString Name);
	
	bool SetState(const FString Name);
	bool ForceSetState(const FString Name);
	bool ForceRollbackState(const FString Name);
		
	static bool CheckStateEntryCondition(const EEntryState EntryState, const int ActionFlags)
	{
		if (EntryState == EEntryState::Standing && ActionFlags & (int)ACT_Standing
			|| EntryState == EEntryState::Crouching && ActionFlags & (int)ACT_Crouching
			|| EntryState == EEntryState::Crouching && ActionFlags & (int)ACT_Standing
			|| EntryState == EEntryState::Jumping && ActionFlags & (int)ACT_Jumping
			|| EntryState == EEntryState::None)
		{
			return true;
		}
		return false;
	}
	
	void Tick(const float DeltaTime)
	{
		if (CurrentState != nullptr)
		{
			CurrentState->OnUpdate(DeltaTime);
		}
	}

	UState* GetCurrentState()
	{
		return CurrentState;
	}
};
