// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FighterGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API AFighterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	bool bAllExistingPlayersRegistered;

	virtual void PostLogin(APlayerController* NewPlayer) override;

public:
	void RegisterExistingPlayers();
	void PreLogout(APlayerController *InPlayerController);
};
