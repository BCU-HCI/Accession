// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "WidgetWrappers/WidgetWrapperBase.h"

#include "AccessionWidgetWrapperRegistry.generated.h"

typedef TFunction<UWidgetWrapper* (const TSharedRef<SWidget>&)> WidgetWrapperFactory;

UCLASS()
class UWidgetWrapperRegistry : public UObject
{
	GENERATED_BODY()

public:

	virtual ~UWidgetWrapperRegistry() override;

	void Initialize();

	bool Tick(float delta);

	bool RegisterWidgetWrapper(const FName WidgetTypeToWrap, const WidgetWrapperFactory& WidgetWrapperFactory, bool ForceOverride = false);
	bool RegisterWidgetWrapper(const TSharedRef<SWidget>& WidgetToWrap, const WidgetWrapperFactory& WidgetWrapperFactory, bool ForceOverride = false);

	bool UnregisterWidgetWrapper(const FName WidgetTypeToRemove);
	bool UnregisterWidgetWrapper(const TSharedRef<SWidget>& WidgetToRemove);

	bool CreateWidgetWrapper(const TSharedRef<SWidget>& WidgetToWrap, UWidgetWrapper* WidgetWrapper);

private:

	bool ResolveWidgetAncestor(TSharedRef<SWidget>& Widget, const WidgetWrapperFactory WidgetWrapper);

private:
	FTSTicker::FDelegateHandle TickDelegateHandle;

	TMap<FName, WidgetWrapperFactory> WidgetWrapperFactories;

	TArray<UWidgetWrapper*> CreatedWidgetWrappers;
};