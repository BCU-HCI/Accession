// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWrappers/AccessibilityAddNodeContextMenu.h"
#include "OpenAccessibilityLogging.h"

#include "Widgets/Input/SSearchBox.h"

#include "Styling/AppStyle.h"

UAccessibilityAddNodeContextMenu::UAccessibilityAddNodeContextMenu(TSharedRef<IMenu> Menu)
: UAccessibilityContextMenu(Menu)
{

}

UAccessibilityAddNodeContextMenu::UAccessibilityAddNodeContextMenu(TSharedRef<IMenu> Menu, TSharedRef<SGraphActionMenu> GraphMenu)
: UAccessibilityContextMenu(Menu)
{
	this->GraphMenu = GraphMenu;
	this->FilterTextBox = GraphMenu->GetFilterTextBox();
}

UAccessibilityAddNodeContextMenu::UAccessibilityAddNodeContextMenu(TSharedRef<IMenu> Menu, TSharedRef<SGraphActionMenu> GraphMenu, TSharedRef<STreeView<TSharedPtr<FGraphActionNode>>> TreeView)
: UAccessibilityContextMenu(Menu)
{
	this->GraphMenu = GraphMenu;
	this->TreeView = TreeView;
	this->FilterTextBox = GraphMenu->GetFilterTextBox();
}

UAccessibilityAddNodeContextMenu::~UAccessibilityAddNodeContextMenu()
{

}

void UAccessibilityAddNodeContextMenu::Init(TSharedRef<IMenu> InMenu)
{
	UAccessibilityContextMenu::Init(InMenu);

	TSharedPtr<SWidget> KeyboardFocusedWidget = StaticCastSharedPtr<SEditableText>(
		FSlateApplication::Get().GetKeyboardFocusedWidget()
	);
	if (!KeyboardFocusedWidget.IsValid())
	{
		return;
	}

	GraphMenu = StaticCastSharedPtr<SGraphActionMenu>(
		KeyboardFocusedWidget
		->GetParentWidget()
		->GetParentWidget()
		->GetParentWidget()
		->GetParentWidget()
		->GetParentWidget()
	);

	{
		TSharedPtr<SSearchBox> SearchBox = StaticCastSharedPtr<SSearchBox>(
			KeyboardFocusedWidget
				->GetParentWidget()
				->GetParentWidget()
				->GetParentWidget()
		);

		TSharedRef<SWidget> SearchBoxSibling = SearchBox->GetParentWidget()->GetChildren()->GetChildAt(1);
		TreeView = StaticCastSharedRef<STreeView<TSharedPtr<FGraphActionNode>>>(
			SearchBoxSibling->GetChildren()->GetChildAt(0)->GetChildren()->GetChildAt(0)
		);
	}
}

void UAccessibilityAddNodeContextMenu::Init(TSharedRef<IMenu> InMenu, TSharedRef<SGraphActionMenu> InGraphMenu, TSharedRef<STreeView<TSharedPtr<FGraphActionNode>>> InTreeView)
{
	UAccessibilityContextMenu::Init(InMenu);

	this->Menu = InMenu;
	this->GraphMenu = InGraphMenu;
	this->TreeView = InTreeView;
	this->FilterTextBox = InGraphMenu->GetFilterTextBox();
}

bool UAccessibilityAddNodeContextMenu::Tick(float DeltaTime)
{
	if (!GraphMenu.IsValid() || !Menu.IsValid())
		return false;

	if (DoesItemsRequireRefresh())
	{
		RefreshAccessibilityWidgets();
	}

	// Set Previous Vars For Next Tick
	PrevFilterString = FilterTextBox.Pin()->GetText().ToString();
	PrevNumItemsBeingObserved = TreeView.Pin()->GetNumItemsBeingObserved();
	PrevNumGeneratedChildren = TreeView.Pin()->GetNumGeneratedChildren();
	PrevScrollDistance = TreeView.Pin()->GetScrollDistance().Y;

	PrevExpandedItems.Reset();
	TreeView.Pin()->GetExpandedItems(PrevExpandedItems);

	return true;
}

bool UAccessibilityAddNodeContextMenu::Close()
{
	DestroyTicker();
	Menu.Pin()->Dismiss();

	return true;
}

void UAccessibilityAddNodeContextMenu::ScaleMenu(const float ScaleFactor)
{
	// Scale TreeView Element
	{
		TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeViewPtr = TreeView.Pin();

		TreeViewPtr->SetItemHeight(16 * ScaleFactor);
	}

	// Scale Window Element
	{
		TSharedPtr<SWindow> WindowPtr = Window.Pin();

		WindowPtr->SetSizingRule(ESizingRule::UserSized);
		WindowPtr->Resize(WindowPtr->GetSizeInScreen() * ScaleFactor);
	}
}

bool UAccessibilityAddNodeContextMenu::DoesItemsRequireRefresh()
{
	return (
		FilterTextBox.Pin()->GetText().ToString() != PrevFilterString ||
		TreeView.Pin()->GetNumItemsBeingObserved() != PrevNumItemsBeingObserved ||
		TreeView.Pin()->GetNumGeneratedChildren() != PrevNumGeneratedChildren ||
		TreeView.Pin()->GetScrollDistance().Y != PrevScrollDistance
	);
}

void UAccessibilityAddNodeContextMenu::RefreshAccessibilityWidgets()
{
	TSet<TSharedPtr<FGraphActionNode>> ExpandedItems;
	TreeView.Pin()->GetExpandedItems(ExpandedItems);

	TSet<TSharedPtr<FGraphActionNode>> ExpandedItemsOpened = ExpandedItems.Difference(PrevExpandedItems);
	TSet<TSharedPtr<FGraphActionNode>> ExpandedItemsClosed = PrevExpandedItems.Difference(ExpandedItems);

	TArray<TSharedPtr<FGraphActionNode>> Items = TArray<TSharedPtr<FGraphActionNode>>(TreeView.Pin()->GetItems());

	{
		TSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>> ItemWidget = nullptr;

		int32 RebuildIndex = -1;

		while (Items.Num() > 0)
		{
			const TSharedPtr<FGraphActionNode> Item = Items[0];
			Items.RemoveAt(0);

			ItemWidget = StaticCastSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>>(
				TreeView.Pin()->WidgetFromItem(Item)
			);

			if (!ItemWidget.IsValid())
			{
				if (IndexedWidgetSet.Contains(Item.Get()))
					IndexedWidgetSet.Remove(Item.Get());

				continue;
			}

			if (ExpandedItemsOpened.Contains(Item) && RebuildIndex == -1)
			{
				TSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>> RebuildItemWidget = StaticCastSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>>(
					TreeView.Pin()->WidgetFromItem(Item)
				);

				if (RebuildItemWidget.IsValid())
					RebuildIndex = RebuildItemWidget->GetIndexInList();
			}
			else if (ExpandedItemsClosed.Contains(Item))
			{
				TArray<TSharedPtr<FGraphActionNode>> DerrivedNodes;
				Item->GetAllNodes(DerrivedNodes);

				for (auto& Child : DerrivedNodes)
					IndexedWidgetSet.Remove(Child.Get());

				if (RebuildIndex == -1)
				{
					RebuildIndex = ItemWidget->GetIndexInList();
				}
			}

			if (!IndexedWidgetSet.Contains(Item.Get()) || (RebuildIndex != -1 && RebuildIndex < ItemWidget->GetIndexInList()))
			{
				ApplyAccessibilityWidget(Item.ToSharedRef(), ItemWidget.ToSharedRef());

				IndexedWidgetSet.Add(Item.Get());
			}
		}
	}
}

FGraphActionNode* UAccessibilityAddNodeContextMenu::GetGraphActionFromIndex(const int32 InIndex)
{
	TArrayView<const TSharedPtr<FGraphActionNode>> Items = TreeView.Pin()->GetItems();
	
	if (Items.Num() > InIndex)
		return Items[InIndex].Get();

	else return nullptr;
}

void UAccessibilityAddNodeContextMenu::GetGraphActionFromIndex(const int32 InIndex, FGraphActionNode* OutGraphAction)
{
	TArrayView<const TSharedPtr<FGraphActionNode>> Items = TreeView.Pin()->GetItems();

	if (Items.Num() > InIndex)
		OutGraphAction = Items[InIndex].Get();

	else OutGraphAction = nullptr;
}

TSharedPtr<FGraphActionNode> UAccessibilityAddNodeContextMenu::GetGraphActionFromIndexSP(const int32 InIndex)
{
	return TreeView.Pin()->GetItems()[InIndex];
}

void UAccessibilityAddNodeContextMenu::SelectGraphAction(const int32 InIndex)
{
	TSharedPtr<FGraphActionNode> GraphAction = GetGraphActionFromIndexSP(InIndex);

	if (GraphAction.IsValid())
	{
		TreeView.Pin()->Private_OnItemClicked(GraphAction);
	}
	else
	{
		UE_LOG(LogOpenAccessibility, Warning, TEXT("SelectGraphAction: Provided GraphAction is Invalid."));
	}
}

void UAccessibilityAddNodeContextMenu::PerformGraphAction(const int32 InIndex)
{
	TSharedPtr<FGraphActionNode> GraphAction = GetGraphActionFromIndexSP(InIndex);

	if (GraphAction.IsValid())
	{
		TreeView.Pin()->Private_OnItemDoubleClicked(GraphAction);
	}
	else
	{
		UE_LOG(LogOpenAccessibility, Warning, TEXT("PerformGraphAction: Provided GraphAction is Invalid."));
	}
}

void UAccessibilityAddNodeContextMenu::SetFilterText(const FString& InFilterText)
{
	FilterTextBox.Pin()->SetText(FText::FromString(InFilterText));
}

void UAccessibilityAddNodeContextMenu::AppendFilterText(const FString& InFilterText)
{
	FilterTextBox.Pin()->SetText(
		FText::FromString(
			FilterTextBox.Pin()->GetText().ToString() + TEXT(" ") + InFilterText
		)
	);
}

void UAccessibilityAddNodeContextMenu::ResetFilterText()
{
	FilterTextBox.Pin()->SetText(FText::FromString(TEXT("")));
}

void UAccessibilityAddNodeContextMenu::SetScrollDistance(const float InScrollDistance)
{
	TreeView.Pin()->SetScrollOffset(InScrollDistance);
}

void UAccessibilityAddNodeContextMenu::ApplyAccessibilityWidget(TSharedRef<FGraphActionNode> Item, TSharedRef<STableRow<TSharedPtr<FGraphActionNode>>> ItemWidget)
{
	TSharedPtr<SWidget> ItemContent = ItemWidget->GetContent();

	ItemWidget->SetContent(
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoWidth()
		[
			SNew(STextBlock)
				.Text(FText::FromString(TEXT("[" + FString::FromInt(ItemWidget->GetIndexInList()) + "]")))
		]

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoWidth()
		[
			ItemContent.ToSharedRef()
		]
	);

}
