// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphActionMenu.h"
#include "GraphActionNode.h"

#include "AccessiblityWrappers/AccessibilityContextMenu.h"
#include "Indexer/WidgetIndexer.h"

#include "AccessibilityAddNodeContextMenu.generated.h"

struct FGraphActionNode;

UCLASS()
class UAccessibilityAddNodeContextMenu : public UAccessibilityContextMenu
{
	GENERATED_BODY()

public:

	UAccessibilityAddNodeContextMenu() = default;
	UAccessibilityAddNodeContextMenu(TSharedRef<IMenu> Menu);
	UAccessibilityAddNodeContextMenu(TSharedRef<IMenu> Menu, TSharedRef<SGraphActionMenu> GraphMenu);
	UAccessibilityAddNodeContextMenu(TSharedRef<IMenu> Menu, TSharedRef<SGraphActionMenu> GraphMenu, TSharedRef<STreeView<TSharedPtr<FGraphActionNode>>> TreeView);

	~UAccessibilityAddNodeContextMenu();

	void Init(TSharedRef<IMenu> InMenu, TSharedRef<SGraphActionMenu> InGraphMenu, TSharedRef<STreeView<TSharedPtr<FGraphActionNode>>> InTreeView);

	bool DoesItemsRequireRefresh();
	void RefreshAccessibilityWidgets();

	// UAccessibilityContextMenu Implementation

	virtual bool Tick(float DeltaTime) override;

	// End UAccessibilityContextMenu Implementation

protected:

	// Naive Indexing

	TMap<int32, FGraphActionNode*> IndexMap;
	TSet<FGraphActionNode*> IndexedWidgetSet;

	// Menu Components
	
	TWeakPtr<SGraphActionMenu> GraphMenu;
	TWeakPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeView;
	TWeakPtr<SEditableTextBox> FilterTextBox;

	// Prev Vars

	FString PrevFilterString;
	int32 PrevNumItemsBeingObserved;
};