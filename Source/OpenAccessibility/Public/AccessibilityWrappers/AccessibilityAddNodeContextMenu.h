// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/Containers/ContextMenuObject.h"

#include "SGraphActionMenu.h"
#include "GraphActionNode.h"

#include "AccessibilityAddNodeContextMenu.generated.h"

struct FGraphActionNode;

UCLASS()
class OPENACCESSIBILITY_API UAccessibilityAddNodeContextMenu : public UPhraseTreeContextMenuObject
{
	GENERATED_BODY()

public:

	UAccessibilityAddNodeContextMenu();
	UAccessibilityAddNodeContextMenu(TSharedRef<IMenu> Menu);
	UAccessibilityAddNodeContextMenu(TSharedRef<IMenu> Menu, TSharedRef<SGraphActionMenu> GraphMenu);
	UAccessibilityAddNodeContextMenu(TSharedRef<IMenu> Menu, TSharedRef<SGraphActionMenu> GraphMenu, TSharedRef<STreeView<TSharedPtr<FGraphActionNode>>> TreeView);

	~UAccessibilityAddNodeContextMenu();

	void Init(TSharedRef<IMenu> InMenu, TSharedRef<FPhraseNode> InContextRoot);

	/// <summary>
	/// Initializes the Context Menu from the given components.
	/// </summary>
	/// <param name="InMenu">The Menu Item, for the tragetContext Menu.</param>
	/// <param name="InGraphMenu">The GraphActionMenu, for the target Context Menu.</param>
	/// <param name="InTreeView">The GraphAction TreeView, for the target Context Menu.</param>
	void Init(TSharedRef<IMenu> InMenu, TSharedRef<SGraphActionMenu> InGraphMenu, TSharedRef<STreeView<TSharedPtr<FGraphActionNode>>> InTreeView);

	// -- UAccessibilityContextMenu Implementation

	/// <summary>
	/// Initializes the Context Menu from the given components.
	/// </summary>
	/// <param name="InMenu"></param>
	virtual void Init(TSharedRef<IMenu> InMenu) override;

	virtual bool Tick(float DeltaTime) override;

	/// <summary>
	/// Closes the Context Menu.
	/// </summary>
	/// <returns>Returns True if the Menu was Successfully closed.</returns>
	virtual bool Close() override;

	/// <summary>
	/// Scaled the Context Menu's Core Components based on the provided ScaleFactor.
	/// </summary>
	/// <param name="ScaleFactor">The Factor for Scaling the Context Menu.</param>
	void ScaleMenu(const float ScaleFactor = 1.5f) override;

	// -- End UAccessibilityContextMenu Implementation

	/// <summary>
	/// Does the Context Menu's TreeView Require a Refresh of Accessibility Widgets.
	/// </summary>
	/// <returns>Returns True if the Context Menu requires change.</returns>
	bool DoesItemsRequireRefresh();

	/// <summary>
	/// Performs a Refresh of the TreeView's Accessibility Widgets.
	/// </summary>
	void RefreshAccessibilityWidgets();

	// Utility Interactions
	// Useful for simplifying common interactions.

	/// <summary>
	/// Gets the GraphActionNode from the given Index.
	/// </summary>
	/// <param name="InIndex">The Index of the Node to Find.</param>
	/// <param name="OutGraphAction">The Found GraphActionNode for the Index, or nullptr.</param>
	void GetGraphActionFromIndex(const int32 InIndex, FGraphActionNode* OutGraphAction);

	/// <summary>
	/// Gets the GraphActionNode from the given Index.
	/// </summary>
	/// <param name="InIndex">The Index of the Node to Find.</param>
	/// <returns>The Found GraphActionNode for the Index, or nullptr.</returns>
	FGraphActionNode* GetGraphActionFromIndex(const int32 InIndex);
	
	/// <summary>
	/// Gets the GraphActionNode from the given Index.
	/// </summary>
	/// <param name="InIndex">The Index of the Node to Find.</param>
	/// <returns>The Found GraphActionNode for the Index, or nullptr.</returns>
	TSharedPtr<FGraphActionNode> GetGraphActionFromIndexSP(const int32 InIndex);

	/// <summary>
	/// Performs a Selction in the TreeView, based on the given Index.
	/// </summary>
	/// <param name="InIndex"></param>
	void SelectGraphAction(const int32 InIndex);
	
	/// <summary>
	/// Performs the Action to the Linked GraphActionNode, based on the given Index.
	/// </summary>
	/// <param name="InIndex"></param>
	void PerformGraphAction(const int32 InIndex);

	/// <summary>
	/// Overrides the Current Filter Text with the given string.
	/// </summary>
	/// <param name="InFilterText">The String to Override with.</param>
	void SetFilterText(const FString& InFilterText);

	/// <summary>
	/// Append the given string to the End of the Current Filter Text.
	/// </summary>
	/// <param name="InFilterText">The Text To Append to the End.</param>
	void AppendFilterText(const FString& InFilterText);

	/// <summary>
	/// Clears the Current Filter Text.
	/// </summary>
	void ResetFilterText();

	void SetScrollDistance(const float InScrollDistance);

	void AppendScrollDistance(const float InScrollDistance);

	void SetScrollDistanceTop();

	void SetScrollDistanceBottom();

protected:

	/// <summary>
	/// Applies the Accessibility Visuals to the given Item's TableRow Widget.
	/// </summary>
	/// <param name="Item">The Item to apply to.</param>
	/// <param name="ItemWidget">The Items linked widget.</param>
	void ApplyAccessibilityWidget(TSharedRef<FGraphActionNode> Item, TSharedRef<STableRow<TSharedPtr<FGraphActionNode>>> ItemWidget);

public:

	// Menu Components
	TWeakPtr<SGraphActionMenu> GraphMenu;
	TWeakPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeView;
	TWeakPtr<SEditableTextBox> FilterTextBox;

protected:

	TSet<FGraphActionNode*> IndexedWidgetSet;

	// Prev Vars

	FString PrevFilterString;
	int32 PrevNumItemsBeingObserved;
	int32 PrevNumGeneratedChildren;
	double PrevScrollDistance;

	TSet<TSharedPtr<FGraphActionNode>> PrevExpandedItems;
};