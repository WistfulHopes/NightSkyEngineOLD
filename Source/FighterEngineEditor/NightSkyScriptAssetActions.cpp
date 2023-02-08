#include "NightSkyScriptAssetActions.h"

#include "EditorReimportHandler.h"
#include "FighterEngine/Battle/NightSkyBinary.h"

bool FNightSkyScriptAssetActions::CanFilter()
{
    return true;
}

uint32 FNightSkyScriptAssetActions::GetCategories()
{
    return EAssetTypeCategories::Misc;
}

FText FNightSkyScriptAssetActions::GetName() const
{
    return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_NightSkyScript", "Night Sky Script Asset");
}

UClass* FNightSkyScriptAssetActions::GetSupportedClass() const
{
    return UNightSkyScript::StaticClass();
}

FColor FNightSkyScriptAssetActions::GetTypeColor() const
{
    return FColor(0x4B, 0x00, 0x82);
}

void FNightSkyScriptAssetActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
    FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
    FText ButtonLabel=FText::FromString("Reimport");
    FText ButtonToolTip=FText::FromString("Replaces this Night Sky Script asset with a source on disk");

    auto Scripts = GetTypedWeakObjectPtrs<UNightSkyScript>(InObjects);

    //The function to call on click. Our CallBackFunction. It will pass the selected BcgAsets on to our callback function.
    auto TDelegateExecuteAction=FExecuteAction::CreateSP(this,&FNightSkyScriptAssetActions::ReimportScript, Scripts);

    auto UIAction=FUIAction(TDelegateExecuteAction);

    MenuBuilder.AddMenuEntry(ButtonLabel,ButtonToolTip,FSlateIcon(), UIAction);
}

bool FNightSkyScriptAssetActions::HasActions(const TArray<UObject*>& InObjects) const
{
    return true;
}

void FNightSkyScriptAssetActions::ReimportScript(TArray<TWeakObjectPtr<UNightSkyScript>> ScriptWeakPtrs)
{
    for (auto ScriptWeakPtr : ScriptWeakPtrs)
    {
        UNightSkyScript* Script = ScriptWeakPtr.Get();
        if (Script)
        {
            FReimportManager::Instance()->Reimport(Script, true);
        }
    }
}
