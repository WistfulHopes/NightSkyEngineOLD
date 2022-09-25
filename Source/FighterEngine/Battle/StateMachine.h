// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State.h"
#include "Bitflags.h"
#include "StateMachine.generated.h"

class APlayerCharacter; //forward declaration

/**
 * 
 */
USTRUCT()
struct FIGHTERENGINE_API FStateMachine
{
	GENERATED_BODY()
	
	UPROPERTY()
	UState* CurrentState;

	UPROPERTY()
	TArray<UState*> States;

	UPROPERTY()
	TArray<FString> StateNames;

	UPROPERTY()
	APlayerCharacter* Parent;

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
		
	static bool CheckStateEntryCondition(const EEntryState EntryState, const int ActionFlags);
	
	void Tick(const float DeltaTime)
	{
		if (CurrentState != nullptr)
		{
			CurrentState->OnUpdate(DeltaTime);
		}
	}
};
