// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintState.h"

#include "Actors/BattleActor.h"
#include "Actors/PlayerCharacter.h"

BlueprintState::BlueprintState(UState* InState)
{
	Owner = InState;
}

void BlueprintState::OnEnter()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->OnLoadGameState();
	else if (Owner->Parent)
		Owner->Parent->OnLoadGameState();
	Owner->OnEnter();
}

void BlueprintState::OnUpdate(float DeltaTime)
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->OnLoadGameState();
	else if (Owner->Parent)
		Owner->Parent->OnLoadGameState();
	Owner->OnUpdate(DeltaTime);
}

void BlueprintState::OnExit()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->OnLoadGameState();
	else if (Owner->Parent)
		Owner->Parent->OnLoadGameState();
	Owner->OnExit();
}

void BlueprintState::OnLanding()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->OnLoadGameState();
	else if (Owner->Parent)
		Owner->Parent->OnLoadGameState();
	Owner->OnLanding();
}

void BlueprintState::OnHit()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->OnLoadGameState();
	else if (Owner->Parent)
		Owner->Parent->OnLoadGameState();
	Owner->OnHit();
}

void BlueprintState::OnBlock()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->OnLoadGameState();
	else if (Owner->Parent)
		Owner->Parent->OnLoadGameState();
	Owner->OnBlock();
}

void BlueprintState::OnHitOrBlock()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->OnLoadGameState();
	else if (Owner->Parent)
		Owner->Parent->OnLoadGameState();
	Owner->OnHitOrBlock();
}

void BlueprintState::OnCounterHit()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->OnLoadGameState();
	else if (Owner->Parent)
		Owner->Parent->OnLoadGameState();
	Owner->OnCounterHit();
}

void BlueprintState::OnSuperFreeze()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->OnLoadGameState();
	else if (Owner->Parent)
		Owner->Parent->OnLoadGameState();
	Owner->OnSuperFreeze();
}

void BlueprintState::OnSuperFreezeEnd()
{
	if (Owner->ObjectParent)
		Owner->ObjectParent->OnLoadGameState();
	else if (Owner->Parent)
		Owner->Parent->OnLoadGameState();
	Owner->OnSuperFreezeEnd();
}
