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

	if (FSlateApplication::Get().GetActiveTopLevelRegularWindow().IsValid())
	{
		VisPosition = FSlateApplication::Get().GetActiveTopLevelRegularWindow()->GetPositionInScreen();
	}
	VisPosition.X = DisplayMetrics.PrimaryDisplayWidth;
	VisPosition.Y = DisplayMetrics.PrimaryDisplayHeight;

	TSharedRef<SWindow> MenuWindow = SNew(SWindow)
		.Type(EWindowType::Normal)
		.SizingRule(ESizingRule::Autosized)
		.ScreenPosition(VisPosition)
		.ClientSize(FVector2D(10, 10))
		.IsPopupWindow(true)
		//.InitialOpacity(0.5f)
		.SupportsTransparency(EWindowTransparency::PerWindow)
		.ActivationPolicy(EWindowActivationPolicy::Always)
		.AdjustInitialSizeAndPositionForDPIScale(true)
		[
			MenuContent.ToSharedRef()
		];

	TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();

	MenuWindow->AssignParentWidget(TopLevelWindow);
	FSlateApplication::Get().AddWindowAsNativeChild(MenuWindow , TopLevelWindow.ToSharedRef(), true);

	VisWindow = MenuWindow.ToWeakPtr();
	VisContent = MenuContent.ToWeakPtr();
}

void FTranscriptionVisualizer::UpdateVisualizer()
{
	if (FSlateApplication::Get().IsActive())
	{
		VisWindow.Pin()->ShowWindow();

		// ReparentWindow();

		MoveVisualizer();
	}
	else VisWindow.Pin()->HideWindow();
}

void FTranscriptionVisualizer::ReparentWindow()
{
	TSharedPtr<SWindow> TopLevelActiveWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();
	if (!TopLevelActiveWindow.IsValid())
		return;

	TSharedPtr<SWindow> VisWindowPtr = VisWindow.Pin();

	if (TopLevelActiveWindow == VisWindow.Pin() ||
		TopLevelActiveWindow->GetContent() == VisWindowPtr->GetParentWidget())
		return;

	TSharedPtr<SWindow> PrevParentWindow = VisWindowPtr->GetParentWindow();
	if (PrevParentWindow.IsValid())
	{
		PrevParentWindow->RemoveDescendantWindow(VisWindowPtr.ToSharedRef());
	}

	VisWindowPtr->AssignParentWidget(TopLevelActiveWindow);
	TopLevelActiveWindow->AddChildWindow(VisWindowPtr.ToSharedRef());
}

void FTranscriptionVisualizer::MoveVisualizer()
{
	FVector2D NewPosition = FVector2D();

	if (!GetTopScreenVisualizerPosition(NewPosition))
	{
		GetDisplayVisualizerPosition(NewPosition);
	}

	VisWindow.Pin()->MoveWindowTo(NewPosition);
}

void FTranscriptionVisualizer::OnTranscriptionRecieved(TArray<FString> InTranscription)
{
	for (int i = 0; i < InTranscription.Num(); i++)
	{
		VisContent.Pin()->UpdateTopTranscription(InTranscription[i]);
	}
}

bool FTranscriptionVisualizer::GetTopScreenVisualizerPosition(FVector2D& OutPosition)
{
	TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();
	if (!TopLevelWindow.IsValid())
		return false;

	FVector2D ActiveWindowPosition = TopLevelWindow->GetPositionInScreen();
	FVector2D ActiveWindowBounds = TopLevelWindow->GetClientSizeInScreen();

	TSharedPtr<SWindow> VisWindowPtr = VisWindow.Pin();

	OutPosition.X = (ActiveWindowPosition.X + ActiveWindowBounds.X / 2) - (VisWindowPtr->GetClientSizeInScreen().X / 2);
	OutPosition.Y = (ActiveWindowPosition.Y + ActiveWindowBounds.Y - 50) - VisWindowPtr->GetClientSizeInScreen().Y;

	return true;
}

bool FTranscriptionVisualizer::GetDisplayVisualizerPosition(FVector2D& OutPosition)
{
	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

	OutPosition.X = DisplayMetrics.PrimaryDisplayWidth;
	OutPosition.Y = DisplayMetrics.PrimaryDisplayHeight;
	
	return true;
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
