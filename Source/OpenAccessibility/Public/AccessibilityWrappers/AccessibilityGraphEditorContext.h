// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/Containers/ContextMenuObject.h"

#include "SGraphActionMenu.h"
#include "GraphActionNode.h"

#include "AccessibilityGraphEditorContext.generated.h"

class SContentIndexer;

UCLASS()
class OPENACCESSIBILITY_API UAccessibilityGraphEditorContext : public UPhraseTreeContextMenuObject
{
	GENERATED_BODY()

public:

	UAccessibilityGraphEditorContext();
	UAccessibilityGraphEditorContext(TSharedRef<IMenu> Menu);


	// -- UPhraseTreeContextMenuObject Implementation

	virtual void Init(TSharedRef<IMenu> InMenu, TSharedRef<FPhraseNode> InContextRoot) override;

	virtual bool Tick(float DeltaTime) override;

	virtual bool Close() override;

	virtual void ScaleMenu(const float ScaleFactor = 1.5f) override;

	// -- End of UPhraseTreeContextMenuObject Implementation



	// -- Event Actions

	TSharedPtr<FGraphActionNode> GetTreeViewAction(const int32& InIndex);

	void SelectAction(const int32& InIndex);

	void SetFilterText(const FString& NewString);

	void AppendFilterText(const FString& StringToAdd);

	void SetScrollDistance(const float NewDistance);

	void AppendScrollDistance(const float DistanceToAdd);

	void SetScrollDistanceTop();

	void SetScrollDistanceBottom();

protected:

	// Index Utils

	const int32 GetStaticIndexOffset();

	// Component Finders

	bool FindGraphActionMenu(const TSharedRef<SWidget>& SearchRoot);

	bool FindTreeView(const TSharedRef<SWidget>& SearchRoot);

	bool FindCheckBoxes(const TSharedRef<SWidget>& SearchRoot);

	// Component Tickers

	struct FTreeViewTickRequirements
	{
	public:

		FTreeViewTickRequirements()
			: PrevSearchText(FString())
			, PrevNumItemsBeingObserved(-1)
			, PrevNumGeneratedChildren(-1)
			, PrevScrollDistance(-1.00)
		{ }

		FString PrevSearchText;
		int32 PrevNumItemsBeingObserved;
		int32 PrevNumGeneratedChildren;
		double PrevScrollDistance;
	};

	bool TreeViewCanTick();

	bool TreeViewRequiresTick();

	void TickTreeViewAccessibility();

	// Widget Utils

	void UpdateAccessibilityWidget(const TSharedRef<SContentIndexer>& TableRow, const int32& NewIndex);

	const TSharedRef<SContentIndexer> CreateAccessibilityWrapper(const TSharedRef<SWidget>& ContentToWrap, const int32& Index);
	
protected:

	FTreeViewTickRequirements TreeViewTickRequirements;

	TWeakPtr<SGraphActionMenu> GraphMenu = TWeakPtr<SGraphActionMenu>();
	TWeakPtr<SEditableTextBox> FilterTextBox = TWeakPtr<SEditableTextBox>();

	TWeakPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeView = TWeakPtr<STreeView<TSharedPtr<FGraphActionNode>>>();

	TArray<TWeakPtr<SCheckBox>> CheckBoxes = TArray<TWeakPtr<SCheckBox>>();
};