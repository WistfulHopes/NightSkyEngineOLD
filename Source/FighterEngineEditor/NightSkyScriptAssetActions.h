#pragma once

#include "AssetTypeActions_Base.h"
#include "Templates/SharedPointer.h"

class FNightSkyScriptAssetActions : public FAssetTypeActions_Base
{
public:

	FNightSkyScriptAssetActions(const TSharedRef<ISlateStyle>& InStyle);

	virtual bool CanFilter() override;
	virtual uint32 GetCategories() override;
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;

private:

	/** Pointer to the style set to use for toolkits. */
	TSharedRef<ISlateStyle> Style;
};
