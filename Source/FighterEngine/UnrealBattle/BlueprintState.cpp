// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintState.h"

#include "Actors/UnrealBattleActor.h"
#include "Actors/UnrealPlayerCharacter.h"

BlueprintState::BlueprintState(UState* InState)
{
	Owner = InState;
}

void BlueprintState::OnEnter()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnEnter();
}

void BlueprintState::OnUpdate(float DeltaTime)
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnUpdate(DeltaTime);
}

void BlueprintState::OnExit()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnExit();
}

void BlueprintState::OnLanding()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnLanding();
}

void BlueprintState::OnHit()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnHit();
}

void BlueprintState::OnBlock()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnBlock();
}

void BlueprintState::OnHitOrBlock()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnHitOrBlock();
}

void BlueprintState::OnCounterHit()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnCounterHit();
}

void BlueprintState::OnSuperFreeze()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnSuperFreeze();
}

void BlueprintState::OnSuperFreezeEnd()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnSuperFreezeEnd();
}
