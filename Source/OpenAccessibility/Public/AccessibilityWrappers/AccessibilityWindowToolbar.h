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

	void SelectToolbarItem(int32 Index);

private:

	bool ApplyToolbarIndexing(TSharedRef<SWidget> ToolkitWidget, TSharedRef<SWindow> ToolkitWindow);

	// Widget Getters

	TSharedPtr<SBorder> GetWindowContentContainer(TSharedRef<SWindow> WindowToFindContainer);

	bool GetToolKitToolBar(TSharedRef<SWidget> ToolKitWidget, TSharedPtr<SWidget>& OutToolBar);

	void BindTicker();

	void UnbindTicker();

public:

private:

	TWeakPtr<SWindow> LastTopWindow;

	TWeakPtr<SBorder> LastToolkitParent;

	TWeakPtr<SWidget> LastToolkit;

	FIndexer<int32, SMultiBlockBaseWidget*> ToolbarIndex;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};