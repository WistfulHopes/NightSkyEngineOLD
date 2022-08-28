// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FighterRunners.h"
#include "AdvancedFriendsGameInstance.h"
#include "FighterGameInstance.generated.h"

class APlayerCharacter;
enum class ERoundFormat : uint8;
/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API UFighterGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<TSubclassOf<APlayerCharacter>> PlayerList;
	UPROPERTY(BlueprintReadWrite)
	int PlayerIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ERoundFormat RoundFormat;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int StartRoundTimer;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TEnumAsByte<EFighterRunners> FighterRunner;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<ACameraActor> FighterCameraActor;
};
