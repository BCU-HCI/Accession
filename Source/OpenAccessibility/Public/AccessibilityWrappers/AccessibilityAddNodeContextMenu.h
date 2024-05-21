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

	/// <summary>
	/// Initializes the Context Menu.
	/// </summary>
	/// <param name="InMenu">The Menu to Initialize from and obtain key components.</param>
	/// <param name="InContextRoot">The Context Root in the PhraseTree that this ContextMenu Originates from.</param>
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

	/// <summary>
	/// Sets the Scroll Distance of the TreeView.
	/// </summary>
	/// <param name="InScrollDistance">The Value to Set the Scroll Distance to.</param>
	void SetScrollDistance(const float InScrollDistance);

	/// <summary>
	/// Adds the provided value to the Current Scroll Distance.
	/// </summary>
	/// <param name="InScrollDistance">The Scroll Distance to Add the Current Distance. Positive Values are down, with Negative being up.</param>
	void AppendScrollDistance(const float InScrollDistance);

	/// <summary>
	/// Sets the Scroll Distance to the Top of the TreeView. Taking the View to the First Item in the TreeView.
	/// </summary>
	void SetScrollDistanceTop();

	/// <summary>
	/// Sets the Scroll Distance to the Bottom of the TreeView. Taking the View to the Last Item in the TreeView.
	/// </summary>
	void SetScrollDistanceBottom();

	/// <summary>
	/// Toggles the Context Awareness of the Node List.
	/// </summary>
	void ToggleContextAwareness();

protected:

	/// <summary>
	/// Applies the Accessibility Widget to the given Item's TableRow Widget.
	/// </summary>
	/// <param name="Item">The Item to apply to.</param>
	/// <param name="ItemWidget">The Items linked widget.</param>
	void ApplyAccessibilityWidget(TSharedRef<STableRow<TSharedPtr<FGraphActionNode>>> ItemWidget);

	/// <summary>
	/// Updates the previously applied Accessibility Widget, with the new index.
	/// </summary>
	/// <param name="ItemWidget">The Item to update.</param>
	void UpdateAccessibilityWidget(TSharedRef<STableRow<TSharedPtr<FGraphActionNode>>> ItemWidget);

public:

	// Menu Components

	/// <summary>
	/// The SGraphActionMenu for the Context Menu.
	/// </summary>
	TWeakPtr<SGraphActionMenu> GraphMenu;

	/// <summary>
	/// The STreeView for the Context Menu.
	/// </summary>
	TWeakPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeView;

	/// <summary>
	/// The SEditableTextBox for the Context Menu. Used for Filtering through GraphNodes.
	/// </summary>
	TWeakPtr<SEditableTextBox> FilterTextBox;

	/// <summary>
	/// The Context Awareness CheckBox for the Context Menu. Used for toggling Context Awareness, in searching for GraphNodes.
	/// </summary>
	TWeakPtr<SCheckBox> ContextAwarenessCheckBox;

protected:

	/// <summary>
	/// Set Containing the Widgets that have been Indexed.
	/// </summary>
	TSet<FGraphActionNode*> IndexedWidgetSet;

	/// <summary>
	/// Set Containing the Items that have been Expanded int the TreeView.
	/// </summary>
	TSet<TSharedPtr<FGraphActionNode>> PrevExpandedItems;

	FString PrevFilterString;
	int32 PrevNumItemsBeingObserved;
	int32 PrevNumGeneratedChildren;
	double PrevScrollDistance;
};