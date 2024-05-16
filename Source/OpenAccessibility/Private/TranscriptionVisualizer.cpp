// Copyright F-Dudley. All Rights Reserved.

#include "TranscriptionVisualizer.h"

#include "AccessibilityWidgets/SAccessibilityTranscriptionVis.h"

FTranscriptionVisualizer::FTranscriptionVisualizer()
{
	RegisterTicker();
}

FTranscriptionVisualizer::~FTranscriptionVisualizer()
{
	UnregisterTicker();
}

bool FTranscriptionVisualizer::Tick(float DeltaTime)
{
	if (VisWindow.IsValid())
	{
		UpdateVisualizer();
	}
	else if (FSlateApplication::Get().GetActiveTopLevelRegularWindow().IsValid() && FSlateApplication::Get().IsActive())
	{
		ConstructVisualizer();
	}

	return true;
}

void FTranscriptionVisualizer::ConstructVisualizer()
{
	TSharedPtr<SAccessibilityTranscriptionVis> MenuContent = SNew(SAccessibilityTranscriptionVis)
		.VisAmount(2);

	MenuContent->ForceVolatile(true);

	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

	FVector2D VisPosition = FVector2D();
	VisPosition.X = DisplayMetrics.PrimaryDisplayWidth / 2;
	VisPosition.Y = DisplayMetrics.PrimaryDisplayHeight / 2;

	TSharedRef<SWindow> MenuWindow = SNew(SWindow)
		.Type(EWindowType::Normal)
		.SizingRule(ESizingRule::Autosized)
		.ScreenPosition(VisPosition)
		.ClientSize(FVector2D(10, 10))
		.IsPopupWindow(false)
		.bDragAnywhere(true)
		.InitialOpacity(0.5f)
		.SupportsTransparency(EWindowTransparency::PerWindow)
		.ActivationPolicy(EWindowActivationPolicy::Always)
		.AdjustInitialSizeAndPositionForDPIScale(true)
		[
			MenuContent.ToSharedRef()
		];

	VisWindow = FSlateApplication::Get().AddWindowAsNativeChild(MenuWindow, FSlateApplication::Get().GetActiveTopLevelRegularWindow().ToSharedRef(), true);
	VisContent = MenuContent.ToWeakPtr();
}

void FTranscriptionVisualizer::UpdateVisualizer()
{
	if (FSlateApplication::Get().IsActive())
	{
		VisWindow.Pin()->ShowWindow();

		ReparentWindow();

		MoveVisualizer();
	}
	else VisWindow.Pin()->HideWindow();
}

void FTranscriptionVisualizer::ReparentWindow()
{
	TSharedPtr<SWindow> TopLevelActiveWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();
	if (!TopLevelActiveWindow.IsValid())
		return;

	if (TopLevelActiveWindow == VisWindow.Pin() ||
		TopLevelActiveWindow->GetContent() == VisWindow.Pin()->GetParentWidget())
		return;
}

void FTranscriptionVisualizer::MoveVisualizer()
{
	// Do Not Perform a Movement if the Active Window is the Visualizer Window.
	TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();
	if (!TopLevelWindow.IsValid())
		return;

	TSharedRef<SWindow> VisWindowRef = VisWindow.Pin().ToSharedRef();

	if (TopLevelWindow == VisWindowRef)
		return;

	FVector2D ActiveWindowPosition = FSlateApplication::Get().GetActiveTopLevelRegularWindow()->GetPositionInScreen();
	FVector2D ActiveWindowBounds = FSlateApplication::Get().GetActiveTopLevelRegularWindow()->GetClientSizeInScreen();

	{
		FVector2D NewPosition = FVector2D();

		NewPosition.X = (ActiveWindowPosition.X + ActiveWindowBounds.X / 2)  - (VisWindowRef->GetClientSizeInScreen().X / 2);
		NewPosition.Y = (ActiveWindowPosition.Y + ActiveWindowBounds.Y - 50) - VisWindowRef->GetClientSizeInScreen().Y;

		VisWindowRef->MoveWindowTo(NewPosition);
	}
}

void FTranscriptionVisualizer::OnTranscriptionRecieved(TArray<FString> InTranscription)
{
	for (int i = 0; i < InTranscription.Num(); i++)
	{
		VisContent.Pin()->UpdateTopTranscription(InTranscription[i]);
	}
}

void FTranscriptionVisualizer::RegisterTicker()
{
	FTickerDelegate TickDelegate = FTickerDelegate::CreateRaw(this, &FTranscriptionVisualizer::Tick);

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);
}

void FTranscriptionVisualizer::UnregisterTicker()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}
