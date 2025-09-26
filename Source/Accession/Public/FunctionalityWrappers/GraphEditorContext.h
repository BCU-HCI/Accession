// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/Containers/ContextMenuObject.h"

#include "SGraphActionMenu.h"
#include "GraphActionNode.h"

#include "GraphEditorContext.generated.h"

class SContentIndexer;

/**
 * A Dynamic Phrase Tree Context Object for Most Node Editor Based Context Menus.
 */
UCLASS()
class ACCESSION_API UGraphEditorContext : public UPhraseTreeContextMenuObject
{
	GENERATED_BODY()

public:
	UGraphEditorContext();

	// -- UPhraseTreeContextMenuObject Implementation

	/**
	 * Initializes the Graph Editor Context Wrapper.
	 * @param InMenu The Interface of the Graph Editor Context Menu.
	 * @param InContextRoot A Reference to the Originating PhraseNode of this Context Object.
	 */
	virtual void Init(TSharedRef<IMenu> InMenu, TSharedRef<FPhraseNode> InContextRoot) override;

	virtual bool Tick(float DeltaTime) override;

	/**
	 * Closes the Graph Editor Context Wrapper Instance.
	 * @return True on successful Closing of the Context Menu, False on Failure.
	 */
	virtual bool Close() override;

	/**
	 * Scales Elements of the Context Menu, by the provided Scalar.
	 * @param ScaleFactor The Scalar to Scale Menu Elements By. (1.5 by Default)
	 */
	virtual void ScaleMenu(const float ScaleFactor = 1.5f) override;

	// -- End of UPhraseTreeContextMenuObject Implementation

	// -- Event Actions

	/**
	 * Gets an Action on the Active TreeView, based on the provided Index.
	 * @param InIndex The Index of the TreeView Action to Find.
	 * @return A Valid Shared Pointer of the Found Action, an Invalid Shared Pointer on Failure.
	 */
	TSharedPtr<FGraphActionNode> GetTreeViewAction(const int32 &InIndex);

	/**
	 * Selects the Action on the Graph Editor Context Menu, based on the given index.
	 * @param InIndex The Index of the Action To Perform.
	 */
	void SelectAction(const int32 &InIndex);

	/**
	 * Gets Filter Text of the Context Menus SearchBar, if it contains one.
	 * @return The Current Filter Text of the Context Menus SearchBar, an Empty String on Failure.
	 */
	FString GetFilterText();

	/**
	 * Sets the Filter Text of the Context Menus SearchBar, if it contains one.
	 * @param NewString The New Text of the SearchBar.
	 */
	void SetFilterText(const FString &NewString);

	/**
	 * Appends the provided string to the Context Menus SearchBar, if it contains one.
	 * @param StringToAdd The Text to Append to the End of the Active SearchBar.
	 */
	void AppendFilterText(const FString &StringToAdd);

	/**
	 * Sets the Scroll Distance of the Context Menus TreeView, if it contains one.
	 * @param NewDistance The New Distance of the Scroll Area.
	 */
	void SetScrollDistance(const float NewDistance);

	/**
	 * Adds the Scroll Distance of the Context Menus TreeView, if it contains one.
	 * @param DistanceToAdd The distance to append to the Scroll Area.
	 */
	void AppendScrollDistance(const float DistanceToAdd);

	/**
	 * Sets the Scroll Distance of the Context Menus TreeView to the Top, if it contains one.
	 */
	void SetScrollDistanceTop();

	/**
	 * Sets the Scroll Distance of the Context Menus TreeView to the Bottom, if it contains one.
	 */
	void SetScrollDistanceBottom();

protected:
	// Index Utils

	/**
	 * Gets the Offset in Indexes of Found Static Components of the Context Menu.
	 * @return The Offset of the Static Components Indexes.
	 */
	const int32 GetStaticIndexOffset();

	// Component Finders

	/**
	 * Finds the SGraphActionMenu Widget descending from the provided widget.
	 * @param SearchRoot The Starting Point for the Widget Search.
	 * @return True if a GraphActionMenu Widget was Found, otherwise False.
	 */
	bool FindGraphActionMenu(const TSharedRef<SWidget> &SearchRoot);

	/**
	 * Finds the STreeView Widget descending from the provided widget.
	 * @param SearchRoot The Starting Point for the Widget Search.
	 * @return True if a TreeView Widget was Found, otherwise False.
	 */
	bool FindTreeView(const TSharedRef<SWidget> &SearchRoot);

	/**
	 * Finds any Static Components of the Context Menu and sorts them into the necessary arrays.
	 * @param SearchRoot The Starting Point for the Widget Search.
	 * @return True if Static Components were Found, otherwise False.
	 */
	bool FindStaticComponents(const TSharedRef<SWidget> &SearchRoot);

	// Component Tickers

	struct FTreeViewTickRequirements
	{
	public:
		FTreeViewTickRequirements()
			: PrevSearchText(FString()), PrevNumItemsBeingObserved(-1), PrevNumGeneratedChildren(-1), PrevScrollDistance(-1.00)
		{
		}

		FString PrevSearchText;
		int32 PrevNumItemsBeingObserved;
		int32 PrevNumGeneratedChildren;
		double PrevScrollDistance;
	};

	/**
	 * Checks if all required components for ticking the TreeView are available.
	 * @return True if all required components are found for TreeView Ticking, otherwise False.
	 */
	bool TreeViewCanTick();

	/**
	 * Checks if the Dynamic TreeView Indexing Components Require a Refresh.
	 * @return True if the TreeView Indexing Assets Require a Refresh.
	 */
	bool TreeViewRequiresTick();

	/**
	 * Updates the TreeView Indexing Components.
	 */
	void TickTreeView();

	// Widget Utils

	/**
	 * Updates the provided Content Indexer Widget with the given Index.
	 * @param ContextIndexer The Context Indexer Widget to Update.
	 * @param NewIndex The Index to update the Context Indexer With.
	 */
	void UpdateIndexWidget(const TSharedRef<SContentIndexer> &ContextIndexer, const int32 &NewIndex);

	/**
	 * Creates a Content Indexer wrapping the provided Widget.
	 * @param ContentToWrap The Content to Wrap with an Indexer.
	 * @param Index The Index of the Provided Content.
	 * @return A Shared Reference of the created Content Indexer, wrapping the provided Content.
	 */
	const TSharedRef<SContentIndexer> CreateIndexWrapper(const TSharedRef<SWidget> &ContentToWrap, const int32 &Index);

protected:
	FTreeViewTickRequirements TreeViewTickRequirements;

	TWeakPtr<SGraphActionMenu> GraphMenu = TWeakPtr<SGraphActionMenu>();
	TWeakPtr<SEditableTextBox> FilterTextBox = TWeakPtr<SEditableTextBox>();

	TWeakPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeView = TWeakPtr<STreeView<TSharedPtr<FGraphActionNode>>>();

	TArray<TWeakPtr<SCheckBox>> CheckBoxes = TArray<TWeakPtr<SCheckBox>>();
};