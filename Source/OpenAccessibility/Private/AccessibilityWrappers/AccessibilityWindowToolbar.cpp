// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWrappers/AccessibilityWindowToolbar.h"

UAccessibilityWindowToolbar::UAccessibilityWindowToolbar() : UObject()
{

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
	TSharedPtr<SWindow> TopActiveWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();
	if (!TopActiveWindow.IsValid())
		return false;

	TSharedPtr<SWindow> TargetWindowPtr = TargetWindow.Pin();

	if (TopActiveWindow != TargetWindowPtr)
	{
		// Reset Graph
		// Indexer->Empty();

		ApplyToolbarIndexing(TopActiveWindow.ToSharedRef());

		// Add Window To Indexed Set
		// and Bind Close Event to track on close.
		IndexedWindows.Add(TopActiveWindow.Get());
		TopActiveWindow->GetOnWindowClosedEvent()
			.AddLambda([&](const TSharedRef<SWindow> WindowClosing) {
				IndexedWindows.Remove(WindowClosing.ToSharedPtr().Get());
			});

		TargetWindow = TopActiveWindow;
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
