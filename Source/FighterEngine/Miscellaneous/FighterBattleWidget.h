// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FighterBattleWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API UFighterBattleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	TArray<float> P1Health;
	UPROPERTY(BlueprintReadOnly)
	TArray<float> P2Health;
	UPROPERTY(BlueprintReadOnly)
	float Timer;
	UPROPERTY(BlueprintReadOnly)
	float P1Meter;
	UPROPERTY(BlueprintReadOnly)
	float P2Meter;
	UPROPERTY(BlueprintReadOnly)
	float P1Gauge2;
	UPROPERTY(BlueprintReadOnly)
	float P2Gauge2;
	UPROPERTY(BlueprintReadOnly)
	int P1RoundsWon;
	UPROPERTY(BlueprintReadOnly)
	int P2RoundsWon;
	UPROPERTY(BlueprintReadOnly)
	int P1ComboCounter;
	UPROPERTY(BlueprintReadOnly)
	int P2ComboCounter;
};
