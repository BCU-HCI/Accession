// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Indexers/Indexer.h"

#include "AccessibilityWindowToolbar.generated.h"

UCLASS()
class OPENACCESSIBILITY_API UAccessibilityWindowToolbar : public UObject
{
	GENERATED_BODY()

public:

	UAccessibilityWindowToolbar();

	virtual ~UAccessibilityWindowToolbar();

	void Init();

	bool Tick(float DeltaTime);

	void ApplyToolbarIndexing(TSharedRef<SWindow> WindowToApply);

private:

	void BindTicker();

	void UnbindTicker();

public:

private:

	TWeakPtr<SWindow> TargetWindow;

	TWeakPtr<SDockTab> TargetTab;

	TUniquePtr<FIndexer<int32, SMultiBlockBaseWidget*>> Indexer;

	TSet<SDockTab*> IndexedTabs;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};