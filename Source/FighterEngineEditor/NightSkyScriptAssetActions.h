#pragma once

#include "AssetTypeActions_Base.h"

class UNightSkyScript;

class FNightSkyScriptAssetActions : public FAssetTypeActions_Base
{
public:	
	virtual bool CanFilter() override;
	virtual uint32 GetCategories() override;
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
	void ReimportScript(TArray<TWeakObjectPtr<UNightSkyScript>> ScriptWeakPtrs);
};
