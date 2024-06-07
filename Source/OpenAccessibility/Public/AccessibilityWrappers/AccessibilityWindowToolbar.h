// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Indexers/Indexer.h"
#include "PhraseTree/Containers/ParseRecord.h"

#include "AccessibilityWindowToolbar.generated.h"

UCLASS()
class OPENACCESSIBILITY_API UAccessibilityWindowToolbar : public UObject
{
	GENERATED_BODY()

public:

	UAccessibilityWindowToolbar();

	virtual ~UAccessibilityWindowToolbar();

	bool Tick(float DeltaTime);

	// -- Parse Events --

	void SelectToolbarItem(FParseRecord& Record);

private:

	void ApplyToolbarIndexing(TSharedRef<SWidget> ToolkitWidget, TSharedRef<SWindow> ToolkitWindow);

	TSharedPtr<SBorder> GetWindowContentContainer(TSharedRef<SWindow> WindowToFindContainer);

	bool GetToolKitToolBar(TSharedRef<SWidget> ToolKitWidget, TSharedPtr<SWidget>& OutToolBar);

	void BindTicker();

	void UnbindTicker();

public:

private:

	TSet<SWidget*> IndexedToolkits;

	TWeakPtr<SWindow> LastTopWindow;

	TWeakPtr<SBorder> LastToolkitParent;

	TWeakPtr<SWidget> LastToolkit;

	TUniquePtr<FIndexer<int32, SMultiBlockBaseWidget*>> ToolbarIndex;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};