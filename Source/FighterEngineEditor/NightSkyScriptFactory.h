// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorReimportHandler.h"
#include "Factories/Factory.h"
#include "NightSkyScriptFactory.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTERENGINEEDITOR_API UNightSkyScriptFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

	UNightSkyScriptFactory();
	
	virtual UObject* FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn) override;

	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;

	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;

	virtual EReimportResult::Type Reimport(UObject* Obj) override;
};
