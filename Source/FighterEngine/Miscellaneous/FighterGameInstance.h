// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "FighterGameInstance.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API UFighterGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<APlayerCharacter>> PlayerList;
	UPROPERTY(BlueprintReadWrite)
	int PlayerIndex;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsSinglePlayer;
};
