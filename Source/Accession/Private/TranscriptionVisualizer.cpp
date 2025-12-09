// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "TranscriptionVisualizer.h"

#include "AccessionLogging.h"
#include "Widgets/STranscriptionVis.h"

UTranscriptionVisualizer::UTranscriptionVisualizer()
{
	//TranscriptionList.Reserve(MaxVisualisationAmount);

	//RegisterTicker();
}

UTranscriptionVisualizer::~UTranscriptionVisualizer()
{
	UnregisterTicker();
}

bool UTranscriptionVisualizer::Tick(float DeltaTime)
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

void UTranscriptionVisualizer::ConstructVisualizer()
{
	TSharedPtr<STranscriptionVis> MenuContent = SNew(STranscriptionVis)
													.VisAmount(MaxVisualisationAmount);

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
											 [MenuContent.ToSharedRef()];

	TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();

	MenuWindow->AssignParentWidget(TopLevelWindow);
	FSlateApplication::Get().AddWindowAsNativeChild(MenuWindow, TopLevelWindow.ToSharedRef(), true);

	VisWindow = MenuWindow.ToWeakPtr();
	VisContent = MenuContent.ToWeakPtr();
}

void UTranscriptionVisualizer::UpdateVisualizer()
{
	if (FSlateApplication::Get().IsActive())
	{
		VisWindow.Pin()->ShowWindow();

		MoveVisualizer();
	}
	else
		VisWindow.Pin()->HideWindow();

	if (bIsTranscriptionDirty)
	{
		FScopeLock Lock(&TranscriptionQueueMutex);

		for (const auto& TransSlot : TranscriptionList)
		{
			VisContent.Pin()->UpdateTopTranscription(TransSlot);
		}

		bIsTranscriptionDirty = false;
	}
	
}

void UTranscriptionVisualizer::ReparentWindow()
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

void UTranscriptionVisualizer::MoveVisualizer()
{
	FVector2D NewPosition = FVector2D();

	if (!GetTopScreenVisualizerPosition(NewPosition))
	{
		GetDisplayVisualizerPosition(NewPosition);
	}

	VisWindow.Pin()->MoveWindowTo(NewPosition);
}

void UTranscriptionVisualizer::OnTranscriptionRecieved(TArray<FString> InTranscription)
{
	FScopeLock Lock(&TranscriptionQueueMutex);

	FString LastTopItem = FString();
	for (int i = 0; i < InTranscription.Num(); i++)
	{
		if (TranscriptionList.Num() >= MaxVisualisationAmount)
		{
			TranscriptionList.RemoveAt(0, 1, false);
		}

		TranscriptionList.Add(InTranscription[i]);
	}

	bIsTranscriptionDirty = true;
}

bool UTranscriptionVisualizer::GetTopScreenVisualizerPosition(FVector2D &OutPosition)
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

bool UTranscriptionVisualizer::GetDisplayVisualizerPosition(FVector2D &OutPosition)
{
	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

	OutPosition.X = DisplayMetrics.PrimaryDisplayWidth;
	OutPosition.Y = DisplayMetrics.PrimaryDisplayHeight;

	return true;
}

void UTranscriptionVisualizer::RegisterTicker()
{
	FTickerDelegate TickDelegate = FTickerDelegate::CreateUObject(this, &UTranscriptionVisualizer::Tick);

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);
}

void UTranscriptionVisualizer::UnregisterTicker()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}
