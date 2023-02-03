// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FighterAIController.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API AFighterAIController : public AAIController
{
	GENERATED_BODY()

public:
	int Inputs;

	UFUNCTION(BlueprintCallable)
	void PressUp();
	UFUNCTION(BlueprintCallable)
	void ReleaseUp();
	UFUNCTION(BlueprintCallable)
	void PressDown();
	UFUNCTION(BlueprintCallable)
	void ReleaseDown();
	UFUNCTION(BlueprintCallable)
	void PressLeft();
	UFUNCTION(BlueprintCallable)
	void ReleaseLeft();
	UFUNCTION(BlueprintCallable)
	void PressRight();
	UFUNCTION(BlueprintCallable)
	void ReleaseRight();
	UFUNCTION(BlueprintCallable)
	void ReleaseAllDirections();
	UFUNCTION(BlueprintCallable)
	void PressL();
	UFUNCTION(BlueprintCallable)
	void ReleaseL();
	UFUNCTION(BlueprintCallable)
	void PressM();
	UFUNCTION(BlueprintCallable)
	void ReleaseM();
	UFUNCTION(BlueprintCallable)
	void PressH();
	UFUNCTION(BlueprintCallable)
	void ReleaseH();
	UFUNCTION(BlueprintCallable)
	void PressS();
	UFUNCTION(BlueprintCallable)
	void ReleaseS();
	UFUNCTION(BlueprintCallable)
	void ReleaseAllButtons();
};
