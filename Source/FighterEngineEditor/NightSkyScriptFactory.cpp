// Fill out your copyright notice in the Description page of Project Settings.


#include "NightSkyScriptFactory.h"

#include "NightSkyBinaryImportData.h"
#include "Misc/FileHelper.h"

#include "FighterEngine/Battle/NightSkyBinary.h"

UNightSkyScriptFactory::UNightSkyScriptFactory()
{
	Formats.Add(FString(TEXT("nss;")) + NSLOCTEXT("UNightSkyScriptFactory", "FormatNSS", "Night Sky Script File").ToString());
	SupportedClass = UNightSkyScript::StaticClass();

	bCreateNew = false;
	bText = false;
	bEditorImport = true;
}

UObject* UNightSkyScriptFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName,
                                                     EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd,
                                                     FFeedbackContext* Warn)
{
	UNightSkyScript* Script = nullptr;
	TArray<uint8> ScriptData;
	UImportSubsystem* ImportSubsystem = GEditor->GetEditorSubsystem<UImportSubsystem>();

	ImportSubsystem->OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Type);

	if (FFileHelper::LoadFileToArray(ScriptData, *CurrentFilename))
	{
		Script = NewObject<UNightSkyScript>(InParent, InClass, InName, Flags);
		Script->Data = ScriptData;
		if (!Script->ImportData || !Script->ImportData->IsA<UNightSkyBinaryImportData>())
		{
			auto ImportData = NewObject<UNightSkyBinaryImportData>(Script);
			Script->ImportData = ImportData;
			Script->ImportData->Update(GetCurrentFilename());
		}
	}
	ImportSubsystem->OnAssetPostImport.Broadcast(this, Script);
	
	return Script;
}

bool UNightSkyScriptFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UNightSkyScript* Script = Cast<UNightSkyScript>(Obj);
	UAssetImportData* ImportData = Script != nullptr ? Script->ImportData : nullptr;
	if (ImportData != nullptr)
	{
		const auto& SourcePath = ImportData->GetFirstFilename();
		FString Path, Filename, Extension;
		FPaths::Split(SourcePath, Path, Filename, Extension);
		ImportData->ExtractFilenames(OutFilenames);
		return true;
	}
	return false;
}

void UNightSkyScriptFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UNightSkyScript* Script = Cast<UNightSkyScript>(Obj);
	UAssetImportData* ImportData = Script != nullptr ? Script->ImportData : nullptr;
	if (ImportData && ensure(NewReimportPaths.Num() == 1)) {
		ImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UNightSkyScriptFactory::Reimport(UObject* Obj)
{
	UNightSkyScript* Script = Cast<UNightSkyScript>(Obj);
	UAssetImportData* ImportData = Script != nullptr ? Script->ImportData : nullptr;
	if (!ImportData)
		return EReimportResult::Failed;

	const auto& Filename = ImportData->GetFirstFilename();
	if (!Filename.Len() || IFileManager::Get().FileSize(*Filename) == INDEX_NONE)
		return EReimportResult::Failed;

	auto Result = EReimportResult::Failed;
	auto OutCanceled = false;
	if (ImportObject(Obj->GetClass(), Obj->GetOuter(), *Obj->GetName(), RF_Public | RF_Standalone, Filename, nullptr, OutCanceled) != nullptr)
	{
		ImportData->Update(Filename);
		if (Obj->GetOuter())
		{
			Obj->GetOuter()->MarkPackageDirty();
		}
		else
		{
			Obj->MarkPackageDirty();
		}
		Result = EReimportResult::Succeeded;
	}
	else {
		Result = EReimportResult::Failed;
	}
	return Result;
}
