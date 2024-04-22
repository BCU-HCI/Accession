// Copyright F-Dudley. All Rights Reserved.

#include "AccessiblityWrappers/AccessibilityAddNodeContextMenu.h"
#include "OpenAccessibilityLogging.h"

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

void UAccessibilityAddNodeContextMenu::Init(TSharedRef<IMenu> InMenu, TSharedRef<SGraphActionMenu> InGraphMenu, TSharedRef<STreeView<TSharedPtr<FGraphActionNode>>> InTreeView)
{
	UAccessibilityContextMenu::Init(InMenu);

	this->Menu = InMenu;
	this->GraphMenu = InGraphMenu;
	this->TreeView = InTreeView;
	this->FilterTextBox = InGraphMenu->GetFilterTextBox();
}

bool UAccessibilityAddNodeContextMenu::DoesItemsRequireRefresh()
{
	return (
		FilterTextBox.Pin()->GetText().ToString() != PrevFilterString ||
		TreeView.Pin()->GetNumItemsBeingObserved() != PrevNumItemsBeingObserved
	);
}

void UAccessibilityAddNodeContextMenu::RefreshAccessibilityWidgets()
{
	TArrayView<const TSharedPtr<FGraphActionNode>> RootItems = TreeView.Pin()->GetRootItems();

	TSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>> ItemWidget = nullptr;
	TSharedPtr<SWidget> ItemContent = nullptr;

	for (const TSharedPtr<FGraphActionNode>& RootItem : RootItems)
	{
		ItemWidget = StaticCastSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>>(
			TreeView.Pin()->WidgetFromItem(RootItem)
		);

		if (!ItemWidget.IsValid())
		{
			if (IndexedWidgetSet.Contains(RootItem.Get()))
			{
				IndexedWidgetSet.Remove(RootItem.Get());
			}

			return;
		}

		if (IndexedWidgetSet.Contains(RootItem.Get()))
			return;

		ItemContent = ItemWidget->GetContent();
		ItemWidget->SetContent(
			SNew(SHorizontalBox)
			 
			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.AutoWidth()
				[
					SNew(STextBlock)
						.Text(FText::FromString(TEXT("[0]")))
				]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.AutoWidth()
				[
					ItemContent.ToSharedRef()
				]
		);

		IndexedWidgetSet.Add(RootItem.Get());
	}
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

	return true;
}
