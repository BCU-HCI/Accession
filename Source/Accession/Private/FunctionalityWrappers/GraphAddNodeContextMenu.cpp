// Copyright F-Dudley. All Rights Reserved.

#include "FunctionalityWrappers/GraphAddNodeContextMenu.h"
#include "AccessionLogging.h"

#include "Widgets/Input/SSearchBox.h"

#include "Widgets/SContentIndexer.h"

#include "Styling/AppStyle.h"

UGraphAddNodeContextMenu::UGraphAddNodeContextMenu()
	: UPhraseTreeContextMenuObject()
{
}

UGraphAddNodeContextMenu::UGraphAddNodeContextMenu(TSharedRef<IMenu> Menu)
	: UPhraseTreeContextMenuObject(Menu)
{
}

UGraphAddNodeContextMenu::UGraphAddNodeContextMenu(TSharedRef<IMenu> Menu, TSharedRef<SGraphActionMenu> GraphMenu)
	: UPhraseTreeContextMenuObject(Menu)
{
	this->GraphMenu = GraphMenu;
	this->FilterTextBox = GraphMenu->GetFilterTextBox();
}

UGraphAddNodeContextMenu::UGraphAddNodeContextMenu(TSharedRef<IMenu> Menu, TSharedRef<SGraphActionMenu> GraphMenu, TSharedRef<STreeView<TSharedPtr<FGraphActionNode>>> TreeView)
	: UPhraseTreeContextMenuObject(Menu)
{
	this->GraphMenu = GraphMenu;
	this->TreeView = TreeView;
	this->FilterTextBox = GraphMenu->GetFilterTextBox();
}

UGraphAddNodeContextMenu::~UGraphAddNodeContextMenu()
{
}

void UGraphAddNodeContextMenu::Init(TSharedRef<IMenu> InMenu, TSharedRef<FPhraseNode> InContextRoot)
{
	Init(InMenu);

	this->ContextRoot = InContextRoot;
}

void UGraphAddNodeContextMenu::Init(TSharedRef<IMenu> InMenu)
{
	UPhraseTreeContextMenuObject::Init(InMenu);

	// This is a Mess but half the Menu Containers are private, so have to move myself to key aspects of the Menu.

	TSharedPtr<SWidget> KeyboardFocusedWidget = StaticCastSharedPtr<SEditableText>(
		FSlateApplication::Get().GetKeyboardFocusedWidget());
	if (!KeyboardFocusedWidget.IsValid())
	{
		UE_LOG(LogAccession, Warning, TEXT("AddNodeContextWrapper::Init: KeyboardFocusedWidget is Invalid."));
		return;
	}

	this->GraphMenu = StaticCastSharedPtr<SGraphActionMenu>(
		KeyboardFocusedWidget
			->GetParentWidget()
			->GetParentWidget()
			->GetParentWidget()
			->GetParentWidget()
			->GetParentWidget());

	{
		TSharedPtr<SSearchBox> SearchBox = StaticCastSharedPtr<SSearchBox>(
			KeyboardFocusedWidget
				->GetParentWidget()
				->GetParentWidget()
				->GetParentWidget());

		TSharedRef<SWidget> SearchBoxSibling = SearchBox->GetParentWidget()->GetChildren()->GetChildAt(1);
		this->TreeView = StaticCastSharedRef<STreeView<TSharedPtr<FGraphActionNode>>>(
			SearchBoxSibling->GetChildren()->GetChildAt(0)->GetChildren()->GetChildAt(0));
	}

	{
		TSharedRef<SCheckBox> CheckBox = StaticCastSharedRef<SCheckBox>(
			this->GraphMenu.Pin()->GetParentWidget()->GetChildren()->GetChildAt(0)->GetChildren()->GetChildAt(2));

		this->ContextAwarenessCheckBox = CheckBox;
	}

	this->FilterTextBox = this->GraphMenu.Pin()->GetFilterTextBox();

	FSlateApplication::Get().SetKeyboardFocus(this->TreeView.Pin());
}

void UGraphAddNodeContextMenu::Init(TSharedRef<IMenu> InMenu, TSharedRef<SGraphActionMenu> InGraphMenu, TSharedRef<STreeView<TSharedPtr<FGraphActionNode>>> InTreeView)
{
	UPhraseTreeContextMenuObject::Init(InMenu);

	this->GraphMenu = InGraphMenu;
	this->TreeView = InTreeView;
	this->FilterTextBox = InGraphMenu->GetFilterTextBox();
}

bool UGraphAddNodeContextMenu::Tick(float DeltaTime)
{
	if (!GraphMenu.IsValid() || !Menu.IsValid())
		return false;

	if (DoesItemsRequireRefresh())
		RefreshIndexWidgets();

	TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeViewPtr = TreeView.Pin();

	// Set Previous Vars For Next Tick
	PrevFilterString = FilterTextBox.Pin()->GetText().ToString();
	PrevNumItemsBeingObserved = TreeViewPtr->GetNumItemsBeingObserved();
	PrevNumGeneratedChildren = TreeViewPtr->GetNumGeneratedChildren();
	PrevScrollDistance = TreeViewPtr->GetScrollDistance().Y;

	return true;
}

bool UGraphAddNodeContextMenu::Close()
{
	RemoveTickDelegate();
	Menu.Pin()->Dismiss();

	return true;
}

void UGraphAddNodeContextMenu::ScaleMenu(const float ScaleFactor)
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

bool UGraphAddNodeContextMenu::DoesItemsRequireRefresh()
{
	TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeViewPtr = TreeView.Pin();

	return (
		FilterTextBox.Pin()->GetText().ToString() != PrevFilterString ||
		TreeViewPtr->GetNumItemsBeingObserved() != PrevNumItemsBeingObserved ||
		TreeViewPtr->GetNumGeneratedChildren() != PrevNumGeneratedChildren ||
		TreeViewPtr->GetScrollDistance().Y != PrevScrollDistance);
}

void UGraphAddNodeContextMenu::RefreshIndexWidgets()
{

	TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeViewPtr = TreeView.Pin();

	TArray<TSharedPtr<FGraphActionNode>> Items = TArray<TSharedPtr<FGraphActionNode>>(TreeViewPtr->GetRootItems());

	{
		TSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>> ItemWidget = nullptr;

		while (Items.Num() > 0)
		{
			const TSharedPtr<FGraphActionNode> Item = Items[0];
			Items.RemoveAt(0);

			if (TreeViewPtr->IsItemExpanded(Item))
				Items.Append(Item->Children);

			ItemWidget = StaticCastSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>>(
				TreeViewPtr->WidgetFromItem(Item));

			if (!ItemWidget.IsValid())
				continue;

			// TO-DO: Change To Non-HardCoded Type Comparison.
			if (ItemWidget->GetContent()->GetType() == "SContentIndexer")
			{
				UpdateIndexWidget(ItemWidget.ToSharedRef());
			}
			else
			{
				ApplyIndexWidget(ItemWidget.ToSharedRef());
			}
		}
	}
}

FGraphActionNode *UGraphAddNodeContextMenu::GetGraphActionFromIndex(const int32 InIndex)
{
	TArrayView<const TSharedPtr<FGraphActionNode>> Items = TreeView.Pin()->GetItems();

	if (Items.Num() > InIndex)
		return Items[InIndex].Get();

	else
		return nullptr;
}

void UGraphAddNodeContextMenu::GetGraphActionFromIndex(const int32 InIndex, FGraphActionNode *OutGraphAction)
{
	TArrayView<const TSharedPtr<FGraphActionNode>> Items = TreeView.Pin()->GetItems();

	if (Items.Num() > InIndex)
		OutGraphAction = Items[InIndex].Get();

	else
		OutGraphAction = nullptr;
}

TSharedPtr<FGraphActionNode> UGraphAddNodeContextMenu::GetGraphActionFromIndexSP(const int32 InIndex)
{
	if (TreeView.Pin()->GetItems().Num() <= InIndex)
	{
		UE_LOG(LogAccession, Warning, TEXT("GetGraphActionFromIndexSP: Provided Index is Out of Range."));
		return nullptr;
	}
	return TreeView.Pin()->GetItems()[InIndex];
}

void UGraphAddNodeContextMenu::SelectGraphAction(const int32 InIndex)
{
	TSharedPtr<FGraphActionNode> GraphAction = GetGraphActionFromIndexSP(InIndex);

	if (GraphAction.IsValid())
	{
		TreeView.Pin()->Private_OnItemClicked(GraphAction);
	}
	else
	{
		UE_LOG(LogAccession, Warning, TEXT("SelectGraphAction: Provided GraphAction is Invalid."));
	}
}

void UGraphAddNodeContextMenu::PerformGraphAction(const int32 InIndex)
{
	TSharedPtr<FGraphActionNode> GraphAction = GetGraphActionFromIndexSP(InIndex);

	if (!GraphAction.IsValid())
	{
		UE_LOG(LogAccession, Warning, TEXT("PerformGraphAction: Provided GraphAction is Invalid."));
	}

	if (GraphAction->IsActionNode())
	{
		TreeView.Pin()->Private_ClearSelection();
		TreeView.Pin()->Private_SetItemSelection(GraphAction, true, true);
		TreeView.Pin()->Private_SignalSelectionChanged(ESelectInfo::OnMouseClick);
	}
	else
	{
		TreeView.Pin()->Private_OnItemDoubleClicked(GraphAction);
	}
}

FString UGraphAddNodeContextMenu::GetFilterText()
{
	return FilterTextBox.Pin()->GetText().ToString();
}

void UGraphAddNodeContextMenu::SetFilterText(const FString &InFilterText)
{
	FilterTextBox.Pin()->SetText(FText::FromString(InFilterText));
}

void UGraphAddNodeContextMenu::AppendFilterText(const FString &InFilterText)
{
	FilterTextBox.Pin()->SetText(
		FText::FromString(
			FilterTextBox.Pin()->GetText().ToString() + TEXT(" ") + InFilterText));
}

void UGraphAddNodeContextMenu::ResetFilterText()
{
	FilterTextBox.Pin()->SetText(FText::FromString(TEXT("")));
}

void UGraphAddNodeContextMenu::SetScrollDistance(const float InScrollDistance)
{
	TreeView.Pin()->SetScrollOffset(InScrollDistance);
}

void UGraphAddNodeContextMenu::AppendScrollDistance(const float InScrollDistance)
{
	if (TreeView.Pin()->GetScrollOffset() + InScrollDistance < 0.0f)
	{
		TreeView.Pin()->SetScrollOffset(0.0f);
		return;
	}

	TreeView.Pin()->AddScrollOffset(InScrollDistance, true);
}

void UGraphAddNodeContextMenu::SetScrollDistanceTop()
{
	TreeView.Pin()->ScrollToTop();
}

void UGraphAddNodeContextMenu::SetScrollDistanceBottom()
{
	TreeView.Pin()->ScrollToBottom();
}

void UGraphAddNodeContextMenu::ToggleContextAwareness()
{
	ContextAwarenessCheckBox.Pin()->ToggleCheckedState();
}

void UGraphAddNodeContextMenu::ApplyIndexWidget(TSharedRef<STableRow<TSharedPtr<FGraphActionNode>>> ItemWidget)
{
	TSharedPtr<SWidget> ItemContent = ItemWidget->GetContent();

	ItemWidget->SetContent(
		SNew(SContentIndexer)
			.IndexValue(ItemWidget->GetIndexInList())
			.IndexPositionToContent(EIndexerPosition::Left)
			.ContentToIndex(ItemContent));
}

void UGraphAddNodeContextMenu::UpdateIndexWidget(TSharedRef<STableRow<TSharedPtr<FGraphActionNode>>> ItemWidget)
{
	TSharedPtr<SContentIndexer> ItemContent = StaticCastSharedPtr<SContentIndexer>(ItemWidget->GetContent());

	ItemContent->UpdateIndex(ItemWidget->GetIndexInList());
}
