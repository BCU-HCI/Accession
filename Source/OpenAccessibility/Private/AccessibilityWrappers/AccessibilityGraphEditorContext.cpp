// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWrappers/AccessibilityGraphEditorContext.h"

#include "OpenAccessibilityLogging.h"
#include "AccessibilityWidgets/SIndexer.h"
#include "AccessibilityWidgets/SContentIndexer.h"
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

	if (!FindTreeView(WindowRef))
	{
		UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphEditorContext: Cannot Find a STreeView Widget"));
	}
	else
	{
		TreeViewTickRequirements = FTreeViewTickRequirements();
	}

	if (!FindCheckBoxes(WindowRef))
	{
		UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphEditorContext: Cannot Find Any SCheckBox Widgets"));
	}
}

bool UAccessibilityGraphEditorContext::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TreeViewCanTick())
	{
		TickTreeViewAccessibility();

		TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeViewPtr = TreeView.Pin();

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

template<class T = SWidget>
FORCEINLINE TSharedPtr<T> GetWidgetDescendant(const TSharedRef<SWidget>& SearchRoot, const FString& TargetWidgetTypeName)
{
	if (SearchRoot->GetType() == TargetWidgetTypeName)
		return StaticCastSharedRef<T>(SearchRoot);

	TArray<FChildren*> ChildrenToSearch = TArray {
		SearchRoot->GetChildren()
	};

	TSharedPtr<SWidget> CurrentChild;
	while (ChildrenToSearch.Num() > 0)
	{
		FChildren* Children = ChildrenToSearch.Pop();

		for (int i = 0; i < Children->Num(); i++)
		{
			CurrentChild = Children->GetChildAt(i);

			if (CurrentChild->GetTypeAsString() == TargetWidgetTypeName)
			{
				return StaticCastSharedPtr<T>(CurrentChild);
			}

			ChildrenToSearch.Add(CurrentChild->GetChildren());
		}
	}

	return TSharedPtr<T>();
}

template<class T = SWidget>
FORCEINLINE TArray<TSharedPtr<T>> GetWidgetDescendants(const TSharedRef<SWidget>& SearchRoot, const FString& TargetWidgetTypeString)
{
	TArray<TSharedPtr<T>> FoundDescendants = TArray<TSharedPtr<T>>();

	if (SearchRoot->GetTypeAsString() == TargetWidgetTypeString)
		FoundDescendants.Add(
			StaticCastSharedRef<T>(SearchRoot)
		);

	TArray<FChildren*> ChildrenToSearch = TArray {
		SearchRoot->GetChildren()
	};

	TSharedPtr<SWidget> CurrentChild;
	while (ChildrenToSearch.Num() > 0)
	{
		FChildren* Children = ChildrenToSearch.Pop();

		for (int i = 0; i < Children->Num(); i++)
		{
			CurrentChild = Children->GetChildAt(i);

			if (CurrentChild->GetTypeAsString() == TargetWidgetTypeString)
				FoundDescendants.Add(
					StaticCastSharedPtr<T>(CurrentChild)
				);

			ChildrenToSearch.Add(CurrentChild->GetChildren());
		}
	}

	return FoundDescendants;
}

const int32 UAccessibilityGraphEditorContext::GetStaticIndexOffset()
{
	return CheckBoxes.Num();
}

bool UAccessibilityGraphEditorContext::FindGraphActionMenu(const TSharedRef<SWidget>& SearchRoot)
{
	TSharedPtr<SGraphActionMenu> GraphActionMenu = GetWidgetDescendant<SGraphActionMenu>(SearchRoot, "SGraphActionMenu");
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
		"STreeView< TSharedPtr<FGraphActionNode> >"
	);
	if (ContextTreeView.IsValid())
	{
		TreeView = ContextTreeView;

		return true;
	}

	return false;
}

bool UAccessibilityGraphEditorContext::FindCheckBoxes(const TSharedRef<SWidget>& SearchRoot)
{
	TArray<TSharedPtr<SCheckBox>> FoundCheckBoxes = GetWidgetDescendants<SCheckBox>(SearchRoot, "SCheckBox");
	if (!FoundCheckBoxes.IsEmpty())
	{
		for (int i = 0; i < FoundCheckBoxes.Num(); i++)
		{
			TSharedPtr<SCheckBox> CheckBox = FoundCheckBoxes[i];

			CheckBox->SetContent(
				SNew(SIndexer)
				.IndexValue(i)
			);
		}

		CheckBoxes.Append(FoundCheckBoxes);

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
		? FilterTextBox.Pin()->GetText().ToString() != GraphMenu.Pin()->LastUsedFilterText
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

	{
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
				UpdateAccessibilityWidget(StaticCastSharedRef<SContentIndexer>(
					ItemContent.ToSharedRef()), 
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

