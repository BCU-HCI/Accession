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

	bool Tick(float DeltaTime);

private:

	void ApplyToolbarIndexing(TSharedRef<SWidget> ToolkitWidget);

	TSharedPtr<SBorder> GetWindowContentContainer(TSharedRef<SWindow> WindowToFindContainer);

	void BindTicker();

	void UnbindTicker();

public:

private:

	TWeakPtr<SWindow> LastTopWindow;

	TWeakPtr<SBorder> LastToolkitParent;

	TWeakPtr<SWidget> LastToolkit;

	TUniquePtr<FIndexer<int32, SMultiBlockBaseWidget*>> ToolbarIndex;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};