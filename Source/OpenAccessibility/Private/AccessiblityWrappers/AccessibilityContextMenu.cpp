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
		DestroyTicker();
	});
}

UAccessibilityContextMenu::~UAccessibilityContextMenu()
{
	// Unbind Tick Delegate
	DestroyTicker();
}

void UAccessibilityContextMenu::Init(TSharedRef<IMenu> InMenu)
{
	this->Menu = Menu;
	this->Window = FSlateApplication::Get().FindWidgetWindow(
		InMenu->GetContent().ToSharedRef()
	);

	TickDelegate = FTickerDelegate::CreateUObject(this, &UAccessibilityContextMenu::Tick);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);
}

void UAccessibilityContextMenu::DestroyTicker()
{
	if (TickDelegateHandle != NULL)
		FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}
