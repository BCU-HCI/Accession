// Copyright F-Dudley. All Rights Reserved.

#include "PhraseTree/Containers/ContextMenuObject.h"

#include "OpenAccessibilityComLogging.h"

UPhraseTreeContextMenuObject::UPhraseTreeContextMenuObject()
	: UPhraseTreeContextObject()
{

}

UPhraseTreeContextMenuObject::UPhraseTreeContextMenuObject(TSharedRef<IMenu> Menu) 
	: UPhraseTreeContextObject()
{

}

UPhraseTreeContextMenuObject::~UPhraseTreeContextMenuObject()
{
	// Unbind Tick Delegate
	RemoveTickDelegate();

	if (Menu.IsValid())
		RemoveMenuDismissed(Menu.Pin().ToSharedRef());

	UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Context Menu || Destroyed ||"))
}

void UPhraseTreeContextMenuObject::Init(TSharedRef<IMenu> InMenu)
{
	this->Menu = InMenu;
	this->Window = FSlateApplication::Get().FindWidgetWindow(
		InMenu->GetContent().ToSharedRef()
	);

	BindMenuDismissed(InMenu);
	BindTickDelegate();
}

void UPhraseTreeContextMenuObject::Init(TSharedRef<IMenu> InMenu, TSharedRef<FPhraseNode> InContextRoot)
{
	this->Menu = InMenu;
	this->Window = FSlateApplication::Get().FindWidgetWindow(
		InMenu->GetContent().ToSharedRef()
	);

	BindMenuDismissed(InMenu);
	BindTickDelegate();
}

void UPhraseTreeContextMenuObject::BindTickDelegate()
{
	TickDelegate = FTickerDelegate::CreateUObject(this, &UPhraseTreeContextMenuObject::Tick);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);
}

void UPhraseTreeContextMenuObject::RemoveTickDelegate()
{
	if (TickDelegateHandle != NULL)
		FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void UPhraseTreeContextMenuObject::BindMenuDismissed(TSharedRef<IMenu> InMenu)
{
	MenuDismissedHandle = InMenu->GetOnMenuDismissed()
		.AddUObject(this, &UPhraseTreeContextMenuObject::OnMenuDismissed);
}

void UPhraseTreeContextMenuObject::RemoveMenuDismissed(TSharedRef<IMenu> InMenu)
{
	Menu.Pin()->GetOnMenuDismissed().Remove(MenuDismissedHandle);
}

void UPhraseTreeContextMenuObject::OnMenuDismissed(TSharedRef<IMenu> InMenu)
{
	RemoveTickDelegate();

	RemoveFromRoot();
	MarkAsGarbage();

	bIsActive = false;

	UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Context Menu || Dismissed ||"))
}
