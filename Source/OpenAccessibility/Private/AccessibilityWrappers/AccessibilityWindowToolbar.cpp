// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWrappers/AccessibilityWindowToolbar.h"

UAccessibilityWindowToolbar::UAccessibilityWindowToolbar() : UObject()
{
	Indexer = MakeUnique<FIndexer<int32, SMultiBlockBaseWidget*>>();
}

UAccessibilityWindowToolbar::~UAccessibilityWindowToolbar()
{
	UnbindTicker();
}

void UAccessibilityWindowToolbar::Init()
{
	BindTicker();
}

bool UAccessibilityWindowToolbar::Tick(float DeltaTime)
{
	// Get Active Tab
	TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
	if (!ActiveTab.IsValid())
		return false;

	TSharedPtr<SDockTab> TargetTabPtr = TargetTab.Pin();

	if (ActiveTab != TargetTabPtr)
	{
		TSharedPtr<SWindow> TabWindow = ActiveTab->GetParentWindow();

		// Empty the Current Indexer
		Indexer->Empty();

		// Apply Indexing to the New Tab, if it is not already Indexed
		if (!IndexedTabs.Contains(ActiveTab.Get()))
		{
			ApplyToolbarIndexing(TabWindow.ToSharedRef());
			IndexedTabs.Add(ActiveTab.Get());
		}

		TargetTab = ActiveTab;
		TargetWindow = TabWindow;
	}

	return true;
}

void UAccessibilityWindowToolbar::ApplyToolbarIndexing(TSharedRef<SWindow> WindowToApply)
{

}

void UAccessibilityWindowToolbar::BindTicker()
{
	FTickerDelegate TickDelegate = FTickerDelegate::CreateUObject(this, &UAccessibilityWindowToolbar::Tick);

	TickDelegateHandle = FTSTicker::GetCoreTicker()
		.AddTicker(TickDelegate);
}

void UAccessibilityWindowToolbar::UnbindTicker()
{
	FTSTicker::GetCoreTicker()
		.RemoveTicker(TickDelegateHandle);
}
