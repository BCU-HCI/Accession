// Copyright F-Dudley. All Rights Reserved.

#include "AccessiblityWrappers/AccessibilityContextMenu.h"

UAccessibilityContextMenu::UAccessibilityContextMenu(TSharedRef<IMenu> Menu) : UObject()
{
	// Bind Tick Delegate
	TickDelegate = FTickerDelegate::CreateUObject(this, &UAccessibilityContextMenu::Tick);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);

	Menu->GetOnMenuDismissed()
	.AddLambda([this](TSharedRef<IMenu> Menu)
	{
		RemoveFromRoot();

		// WidgetIndexer->Clear();
	});
}

UAccessibilityContextMenu::~UAccessibilityContextMenu()
{
	// Unbind Tick Delegate
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void UAccessibilityContextMenu::Init(TSharedRef<IMenu> InMenu)
{
	this->Menu = Menu;

	TickDelegate = FTickerDelegate::CreateUObject(this, &UAccessibilityContextMenu::Tick);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);
}
