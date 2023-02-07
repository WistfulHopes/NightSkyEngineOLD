// Fill out your copyright notice in the Description page of Project Settings.


#include "NightSkyScriptFactory.h"
#include "Misc/FileHelper.h"

#include "FighterEngine/Battle/NightSkyBinary.h"

UNightSkyScriptFactory::UNightSkyScriptFactory()
{
	Formats.Add(FString(TEXT("nss;")) + NSLOCTEXT("UNightSkyScriptFactory", "FormatNSS", "Night Sky Script File").ToString());
	SupportedClass = UNightSkyScript::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}

UObject* UNightSkyScriptFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName,
                                                     EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd,
                                                     FFeedbackContext* Warn)
{
	UNightSkyScript* Script = nullptr;
	TArray<uint8> ScriptData;
	
	if (FFileHelper::LoadFileToArray(ScriptData, *CurrentFilename))
	{
		Script = NewObject<UNightSkyScript>(InParent, InClass, InName, Flags);
		Script->Data = ScriptData;
	}

	return Script;
}
