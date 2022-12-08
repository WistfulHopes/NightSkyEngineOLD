// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintState.h"

#include "Actors/UnrealBattleActor.h"
#include "Actors/UnrealPlayerCharacter.h"

BlueprintState::BlueprintState(UState* InState)
{
	Owner = InState;
}

State* BlueprintState::Clone()
{
	return new BlueprintState(*this); 
}

void BlueprintState::OnEnter()
{
	if (!Owner)
		return;
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnEnter();
	if (Owner->ObjectParent)
		Owner->ObjectParent->Update();
	else if (Owner->Parent)
		Owner->Parent->Update();
}

void BlueprintState::OnUpdate(float DeltaTime)
{
	if (!Owner)
		return;
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnUpdate(DeltaTime);
	if (Owner->ObjectParent)
		Owner->ObjectParent->Update();
	else if (Owner->Parent)
		Owner->Parent->Update();
}

void BlueprintState::OnExit()
{
	if (!Owner)
		return;
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnExit();
	if (Owner->ObjectParent)
		Owner->ObjectParent->Update();
	else if (Owner->Parent)
		Owner->Parent->Update();
}

void BlueprintState::OnLanding()
{
	if (!Owner)
		return;
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnLanding();
	if (Owner->ObjectParent)
		Owner->ObjectParent->Update();
	else if (Owner->Parent)
		Owner->Parent->Update();
}

void BlueprintState::OnHit()
{
	if (!Owner)
		return;
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnHit();
	if (Owner->ObjectParent)
		Owner->ObjectParent->Update();
	else if (Owner->Parent)
		Owner->Parent->Update();
}

void BlueprintState::OnBlock()
{
	if (!Owner)
		return;
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnBlock();
	if (Owner->ObjectParent)
		Owner->ObjectParent->Update();
	else if (Owner->Parent)
		Owner->Parent->Update();
}

void BlueprintState::OnHitOrBlock()
{
	if (!Owner)
		return;
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnHitOrBlock();
	if (Owner->ObjectParent)
		Owner->ObjectParent->Update();
	else if (Owner->Parent)
		Owner->Parent->Update();
}

void BlueprintState::OnCounterHit()
{
	if (!Owner)
		return;
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnCounterHit();
	if (Owner->ObjectParent)
		Owner->ObjectParent->Update();
	else if (Owner->Parent)
		Owner->Parent->Update();
}

void BlueprintState::OnSuperFreeze()
{
	if (!Owner)
		return;
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnSuperFreeze();
	if (Owner->ObjectParent)
		Owner->ObjectParent->Update();
	else if (Owner->Parent)
		Owner->Parent->Update();
}

void BlueprintState::OnSuperFreezeEnd()
{
	if (!Owner)
		return;
	if (Owner->ObjectParent)
		Owner->ObjectParent->PreUpdate();
	else if (Owner->Parent)
		Owner->Parent->PreUpdate();
	Owner->OnSuperFreezeEnd();
	if (Owner->ObjectParent)
		Owner->ObjectParent->Update();
	else if (Owner->Parent)
		Owner->Parent->Update();
}
