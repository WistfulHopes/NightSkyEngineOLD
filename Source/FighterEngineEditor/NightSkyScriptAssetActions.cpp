#include "NightSkyScriptAssetActions.h"

#include "FighterEngine/Battle/NightSkyBinary.h"
#include "Styling/SlateStyle.h"

FNightSkyScriptAssetActions::FNightSkyScriptAssetActions(const TSharedRef<ISlateStyle>& InStyle)
    : Style(InStyle)
{}

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

bool FNightSkyScriptAssetActions::HasActions(const TArray<UObject*>& InObjects) const
{
    return true;
}
