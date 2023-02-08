#include "FighterEngineEditor.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "NightSkyScriptAssetActions.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

IMPLEMENT_GAME_MODULE(FFighterEngineEditorModule, FighterEngineEditor);
DEFINE_LOG_CATEGORY(FighterEngineEditor)

#define LOCTEXT_NAMESPACE "FighterEngineEditor"

void FFighterEngineEditorModule::StartupModule()
{
    IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    auto AssetTypeAction = MakeShareable(new FNightSkyScriptAssetActions());
    AssetToolsModule.RegisterAssetTypeActions(AssetTypeAction);
    
    UE_LOG(FighterEngineEditor, Warning, TEXT("FighterEngineEditor: Log Started"));
}

void FFighterEngineEditorModule::ShutdownModule()
{
    UE_LOG(FighterEngineEditor, Warning, TEXT("FighterEngineEditor: Log Ended"));
}

#undef LOCTEXT_NAMESPACE
