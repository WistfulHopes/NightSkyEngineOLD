// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine.h"

#include "Actors/PlayerCharacter.h"

FString UStateMachine::GetStateName(int Index)
{
	if (Index > 0 && Index < States.Num())
	{
		return States[Index]->Name;
	}
	return "";
}

int UStateMachine::GetStateIndex(FString Name)
{
	return StateNames.Find(Name);
}

bool UStateMachine::SetState(const FString Name)
{
	if (StateNames.Find(Name) == INDEX_NONE)
	{
		return false;
	}

	if (IsCurrentState(Name))
	{
		return false;
	}
		
	CurrentState->OnExit();
	
	CurrentState = States[StateNames.Find(Name)];

	Parent->OnStateChange();
	CurrentState->OnEnter();

	return true;
}

bool UStateMachine::ForceSetState(const FString Name)
{
	if (StateNames.Find(Name) == INDEX_NONE)
	{
		return false;
	}
		
	CurrentState->OnExit();
		
	CurrentState = States[StateNames.Find(Name)];

	Parent->OnStateChange();
	CurrentState->OnEnter();

	return true;
}

bool UStateMachine::ForceRollbackState(const FString Name)
{
	if (StateNames.Find(Name) == INDEX_NONE)
	{
		return false;
	}
		
	CurrentState = States[StateNames.Find(Name)];

	return true;
}
