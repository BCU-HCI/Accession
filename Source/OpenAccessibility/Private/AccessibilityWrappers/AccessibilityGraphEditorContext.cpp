// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWrappers/AccessibilityGraphEditorContext.h"

#include "OpenAccessibilityLogging.h"
#include "AccessibilityWidgets/SIndexer.h"
#include "AccessibilityWidgets/SContentIndexer.h"
#include "Utils/WidgetUtils.h"

#include "Widgets/SWindow.h"
#include "Widgets/Input/SEditableTextBox.h"

UAccessibilityGraphEditorContext::UAccessibilityGraphEditorContext()
	: Super()
{

}

void UAccessibilityGraphEditorContext::Init(TSharedRef<IMenu> InMenu, TSharedRef<FPhraseNode> InContextRoot)
{
	Super::Init(InMenu, InContextRoot);

	TSharedRef<SWindow> WindowRef = Window.Pin().ToSharedRef();

	if (!FindGraphActionMenu(WindowRef))
	{
		UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphEditorContext: Cannot Find a SGraphActionMenu Widget"));
	}

	if (!FindStaticComponents(WindowRef))
	{
		UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphEditorContext: Cannot Find Any Static Components"));
	}

	if (!FindTreeView(WindowRef))
	{
		UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphEditorContext: Cannot Find a STreeView Widget"));
	}
	else
	{
		TreeViewTickRequirements = FTreeViewTickRequirements();
	}
}

bool UAccessibilityGraphEditorContext::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TreeViewCanTick())
	{
		TickTreeViewAccessibility();

		TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeViewPtr = TreeView.Pin();

		TreeViewTickRequirements.PrevSearchText = FilterTextBox.Pin()->GetText().ToString();
		TreeViewTickRequirements.PrevNumGeneratedChildren = TreeViewPtr->GetNumGeneratedChildren();
		TreeViewTickRequirements.PrevNumItemsBeingObserved = TreeViewPtr->GetNumItemsBeingObserved();
		TreeViewTickRequirements.PrevScrollDistance = TreeViewPtr->GetScrollDistance().Y;
	}

	return true;
}

bool UAccessibilityGraphEditorContext::Close()
{
	Super::Close();

	return true;
}

void UAccessibilityGraphEditorContext::ScaleMenu(const float ScaleFactor)
{
	Super::ScaleMenu(ScaleFactor);

	// Scale TreeView
	if (TreeView.IsValid())
	{
		TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeViewPtr = TreeView.Pin();

		TreeViewPtr->SetItemHeight(16 * ScaleFactor);
	}

	// Scale Window Element
	if (Window.IsValid())
	{
		TSharedPtr<SWindow> WindowPtr = Window.Pin();

		WindowPtr->SetSizingRule(ESizingRule::UserSized);
		WindowPtr->Resize(WindowPtr->GetSizeInScreen() * ScaleFactor);
	}
}

TSharedPtr<FGraphActionNode> UAccessibilityGraphEditorContext::GetTreeViewAction(const int32& InIndex)
{
	TArrayView<const TSharedPtr<FGraphActionNode>> Items = TreeView.Pin()->GetItems();

	if (TreeView.IsValid() && Items.Num() > InIndex && InIndex >= 0)
		return TreeView.Pin()->GetItems()[InIndex];

	return TSharedPtr<FGraphActionNode>();
}

void UAccessibilityGraphEditorContext::SelectAction(const int32& InIndex)
{
	if (InIndex < 0)
		return;

	if (!CheckBoxes.IsEmpty() && InIndex < CheckBoxes.Num())
	{
		if (CheckBoxes[InIndex].IsValid())
		{
			CheckBoxes[InIndex].Pin()->ToggleCheckedState();
			return;
		}
	}

	TSharedPtr<FGraphActionNode> ChosenTreeViewAction = GetTreeViewAction(InIndex - GetStaticIndexOffset());
	if (!ChosenTreeViewAction.IsValid())
	{
		UE_LOG(LogOpenAccessibility, Warning, TEXT("SelectGraphAction: Provided TreeView Action is Invalid"))
		return;
	}

	auto TreeViewPtr = TreeView.Pin();
	if (ChosenTreeViewAction->IsActionNode())
	{
		TreeViewPtr->Private_ClearSelection();
		TreeViewPtr->Private_SetItemSelection(ChosenTreeViewAction, true, true);
		TreeViewPtr->Private_SignalSelectionChanged(ESelectInfo::Type::OnMouseClick);
	}
	else
	{
		TreeViewPtr->Private_OnItemDoubleClicked(ChosenTreeViewAction);
	}
}

FString UAccessibilityGraphEditorContext::GetFilterText()
{
	return FilterTextBox.IsValid() ? FilterTextBox.Pin()->GetText().ToString() : FString();
}

void UAccessibilityGraphEditorContext::SetFilterText(const FString& NewString)
{
	if (!FilterTextBox.IsValid())
		return;

	FilterTextBox.Pin()->SetText(
		FText::FromString(NewString)
	);
}

void UAccessibilityGraphEditorContext::AppendFilterText(const FString& StringToAdd)
{
	if (!FilterTextBox.IsValid())
		return;

	TSharedPtr<SEditableTextBox> FilterTextBoxPtr = FilterTextBox.Pin();

	FilterTextBoxPtr->SetText(
		FText::FromString( FilterTextBoxPtr->GetText().ToString() + TEXT(" ") + StringToAdd )
	);
}

void UAccessibilityGraphEditorContext::SetScrollDistance(const float NewDistance)
{
	if (TreeView.IsValid())
		return;

	TreeView.Pin()->SetScrollOffset(NewDistance);
}

void UAccessibilityGraphEditorContext::AppendScrollDistance(const float DistanceToAdd)
{
	auto TreeViewPtr = TreeView.Pin();

	if (TreeViewPtr->GetScrollOffset() + DistanceToAdd < 0.0f)
	{
		TreeViewPtr->SetScrollOffset(0.0f);
		return;
	}

	TreeViewPtr->AddScrollOffset(DistanceToAdd);
}

void UAccessibilityGraphEditorContext::SetScrollDistanceTop()
{
	TreeView.Pin()->ScrollToTop();
}

void UAccessibilityGraphEditorContext::SetScrollDistanceBottom()
{
	TreeView.Pin()->ScrollToBottom();
}

const int32 UAccessibilityGraphEditorContext::GetStaticIndexOffset()
{
	return CheckBoxes.Num();
}

bool UAccessibilityGraphEditorContext::FindGraphActionMenu(const TSharedRef<SWidget>& SearchRoot)
{
	TSharedPtr<SGraphActionMenu> GraphActionMenu = GetWidgetDescendant<SGraphActionMenu>(SearchRoot, TEXT("SGraphActionMenu"));
	if (GraphActionMenu.IsValid())
	{
		GraphMenu = GraphActionMenu;
		FilterTextBox = GraphActionMenu->GetFilterTextBox();

		return true;
	}

	return false;
}

bool UAccessibilityGraphEditorContext::FindTreeView(const TSharedRef<SWidget>& SearchRoot)
{
	TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> ContextTreeView = GetWidgetDescendant<STreeView<TSharedPtr<FGraphActionNode>>>(
		SearchRoot,
		TEXT("STreeView<TSharedPtr<FGraphActionNode>>")
	);
	if (ContextTreeView.IsValid())
	{
		TreeView = ContextTreeView;

		return true;
	}

	return false;
}

bool UAccessibilityGraphEditorContext::FindStaticComponents(const TSharedRef<SWidget>& SearchRoot)
{
	TArray<FSlotBase*> FoundComponentSlots = GetWidgetSlotsByType(
		SearchRoot, 
		TSet<FString> {
			TEXT("SCheckBox")
		}
	);

	if (!FoundComponentSlots.IsEmpty())
	{
		// Sort and Index the Static Components.
		for (int i = 0; i < FoundComponentSlots.Num(); i++)
		{
			FSlotBase* FoundComponentSlot = FoundComponentSlots[i];

			TSharedPtr<SWidget> DetachedWidget = FoundComponentSlot->DetachWidget();
			if (!DetachedWidget.IsValid())
				continue;

			int32 ComponentIndex = -1;
			FString ComponentType = DetachedWidget->GetTypeAsString();

			if (ComponentType == "SCheckBox")
			{
				ComponentIndex = CheckBoxes.Num();
				CheckBoxes.Add(StaticCastSharedPtr<SCheckBox>(DetachedWidget));
			}

			FoundComponentSlot->AttachWidget(
				SNew(SContentIndexer)
				.IndexValue(ComponentIndex)
				.IndexPositionToContent(EIndexerPosition::Left)
				.ContentToIndex(DetachedWidget)
			);
		}

		return true;
	}

	return false;
}

bool UAccessibilityGraphEditorContext::TreeViewCanTick()
{
	return TreeView.IsValid() && GraphMenu.IsValid();
}

bool UAccessibilityGraphEditorContext::TreeViewRequiresTick()
{
	if (!TreeView.IsValid() || !GraphMenu.IsValid())
		return false;

	bool bFilterTextChange = FilterTextBox.IsValid()
		? FilterTextBox.Pin()->GetText().ToString() != TreeViewTickRequirements.PrevSearchText
		: false;

	TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeViewPtr = TreeView.Pin();

	return (
		bFilterTextChange ||
		TreeViewPtr->GetNumItemsBeingObserved() != TreeViewTickRequirements.PrevNumItemsBeingObserved ||
		TreeViewPtr->GetNumGeneratedChildren() != TreeViewTickRequirements.PrevNumGeneratedChildren ||
		TreeViewPtr->GetScrollDistance().Y != TreeViewTickRequirements.PrevScrollDistance
	);
} 

void UAccessibilityGraphEditorContext::TickTreeViewAccessibility()
{
	if (!TreeViewRequiresTick())
		return;

	TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeViewPtr = TreeView.Pin();

	TArray<TSharedPtr<FGraphActionNode>> Items = TArray<TSharedPtr<FGraphActionNode>>(
		TreeViewPtr->GetRootItems()
	);


	TSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>> ItemWidget = nullptr;
	const int32 IndexOffset = GetStaticIndexOffset();

	while (Items.Num() > 0)
	{
		const TSharedPtr<FGraphActionNode> Item = Items[0];
		Items.RemoveAt(0);

		if (TreeViewPtr->IsItemExpanded(Item))
			Items.Append(Item->Children);

		ItemWidget = StaticCastSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>>(
			TreeViewPtr->WidgetFromItem(Item)
		);
		if (!ItemWidget.IsValid())
			continue;

		TSharedPtr<SWidget> ItemContent = ItemWidget->GetContent();

		if (ItemContent->GetType() == "SContentIndexer")
		{
			UpdateAccessibilityWidget(
				StaticCastSharedRef<SContentIndexer>(ItemContent.ToSharedRef()), 
				IndexOffset + ItemWidget->GetIndexInList()
			);
		}
		else
		{
			ItemWidget->SetContent(
				CreateAccessibilityWrapper(ItemContent.ToSharedRef(), IndexOffset + ItemWidget->GetIndexInList())
			);
		}
	}
}

void UAccessibilityGraphEditorContext::UpdateAccessibilityWidget(const TSharedRef<SContentIndexer>& ContentIndexer, const int32& NewIndex)
{
	ContentIndexer->UpdateIndex(NewIndex);
}

const TSharedRef<SContentIndexer> UAccessibilityGraphEditorContext::CreateAccessibilityWrapper(const TSharedRef<SWidget>& ContentToWrap, const int32& Index)
{
	return SNew(SContentIndexer)
		.IndexValue(Index)
		.IndexPositionToContent(EIndexerPosition::Left)
		.ContentToIndex(ContentToWrap);
}

