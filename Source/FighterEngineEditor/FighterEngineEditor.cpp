#include "FighterEngineEditor.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

IMPLEMENT_GAME_MODULE(FFighterEngineEditorModule, FighterEngineEditor);
DEFINE_LOG_CATEGORY(FighterEngineEditor)

#define LOCTEXT_NAMESPACE "FighterEngineEditor"

void FFighterEngineEditorModule::StartupModule()
{
    UE_LOG(FighterEngineEditor, Warning, TEXT("FighterEngineEditor: Log Started"));
}

void FFighterEngineEditorModule::ShutdownModule()
{
    UE_LOG(FighterEngineEditor, Warning, TEXT("FighterEngineEditor: Log Ended"));
}

#undef LOCTEXT_NAMESPACE
