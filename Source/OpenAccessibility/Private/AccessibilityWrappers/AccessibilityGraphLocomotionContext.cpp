// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWrappers/AccessibilityGraphLocomotionContext.h"
#include "OpenAccessibilityLogging.h"

#include "SGraphPanel.h"

UAccessibilityGraphLocomotionContext::UAccessibilityGraphLocomotionContext(const FObjectInitializer& ObjectInitializer)
    : UPhraseTreeContextObject()
{
    LinkedEditor = TWeakPtr<SGraphEditor>();
}

UAccessibilityGraphLocomotionContext::~UAccessibilityGraphLocomotionContext()
{
	Close();

	UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphLocomotion: CONTEXT DESTROYED."));
}

void UAccessibilityGraphLocomotionContext::Init()
{
	{
		TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphLocomotion: NO ACTIVE TAB FOUND."));
			return;
		}

		LinkedEditor = StaticCastSharedRef<SGraphEditor>(ActiveTab->GetContent());
		if (!LinkedEditor.IsValid()) 
		{
			UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphLocomotion: CURRENT ACTIVE TAB IS NOT OF TYPE - SGraphEditor"));
            return;
		}
	}

	TSharedPtr<SGraphEditor> LinkedEditorPtr = LinkedEditor.Pin();

	LinkedEditorPtr->ZoomToFit(false);

	CreateVisualGrid(LinkedEditorPtr.ToSharedRef());
    GenerateVisualChunks(LinkedEditorPtr.ToSharedRef(), FIntVector2(6, 4));
}

void UAccessibilityGraphLocomotionContext::Init(TSharedRef<SGraphEditor> InGraphEditor) 
{
    LinkedEditor = InGraphEditor;

    InGraphEditor->ZoomToFit(false);

    CreateVisualGrid(InGraphEditor);
    GenerateVisualChunks(InGraphEditor, FIntVector2(6, 4));

	HideNativeVisuals();

	BindFocusChangedEvent();
}

void UAccessibilityGraphLocomotionContext::Close() 
{
	UnbindFocusChangedEvent();

	RemoveVisualGrid();
	UnHideNativeVisuals();

	bIsActive = false;

	RemoveFromRoot();
	MarkAsGarbage();

	UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphLocomotion: CONTEXT CLOSED."));
}

void UAccessibilityGraphLocomotionContext::CreateVisualGrid(TSharedRef<SGraphEditor> InGraphEditor) 
{
	TSharedPtr<SOverlay> GraphViewport = StaticCastSharedPtr<SOverlay>(InGraphEditor->GetGraphPanel()->GetParentWidget());
	if (!GraphViewport.IsValid()) 
	{
		UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphLocomotion: NO GRAPH VIEWPORT FOUND."));
		return;
	}

	GridParent = GraphViewport;

	GraphViewport->AddSlot()
	.ZOrder(1)
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SAssignNew(GridContainer, SUniformGridPanel)
	];
}

void UAccessibilityGraphLocomotionContext::GenerateVisualChunks(TSharedRef<SGraphEditor> InGraphEdi, FIntVector2 InVisualChunkSize) 
{
	ChunkArray.Empty();

	TSharedPtr<SUniformGridPanel> GridContainerPtr = GridContainer.Pin();

	FText ChunkText = FText::GetEmpty();

	for (int32 X = 0; X < InVisualChunkSize.X; X++)
	{
		for (int32 Y = 0; Y < InVisualChunkSize.Y; Y++)
		{
			ChunkText = FText::FromString(
				FString::Printf(TEXT("Chunk %d-%d"), X, Y)
			);

			GridContainerPtr->AddSlot(X, Y)
			[
				SNew(SBox)
				[
					SNew(SBorder)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.BorderBackgroundColor(FLinearColor::Green)
					[
						SNew(SBorder)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.Padding(2.0f)
						.BorderBackgroundColor(FLinearColor::Green)
						[
							SNew(STextBlock)
							.Text(ChunkText)
							.ColorAndOpacity(FSlateColor(FLinearColor::Green))
						]
					]
				]
			];
		}
	}
}

void UAccessibilityGraphLocomotionContext::RemoveVisualGrid() 
{
	TSharedPtr<SUniformGridPanel> GridContainerPtr = GridContainer.Pin();
	if (GridContainerPtr.IsValid()) 
	{
        TSharedPtr<SOverlay> ParentWidget = StaticCastSharedPtr<SOverlay>(
			GridContainerPtr->GetParentWidget()
		);
                
		if (ParentWidget.IsValid()) {
			ParentWidget->RemoveSlot(GridContainerPtr.ToSharedRef());

			GridParent = ParentWidget;
        }
		else UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphLocomotion: PARENT WIDGET NOT FOUND, CANNOT REMOVE LOCOMOTION WIDGETS."))
    }
}

void UAccessibilityGraphLocomotionContext::HideNativeVisuals() 
{
    NativeWidgetVisibility.Empty();

	TSharedPtr<SOverlay> GraphViewport = GridParent.Pin();
    TSharedPtr<SUniformGridPanel> VisualGrid = GridContainer.Pin();
	SGraphPanel* GraphPanel = LinkedEditor.Pin()->GetGraphPanel();

	FChildren* ViewportChildren = GraphViewport->GetChildren();

	TSharedPtr<SWidget> ChildWidget;
	for (int32 i = 0; i < ViewportChildren->Num(); i++)
	{
        ChildWidget = ViewportChildren->GetChildAt(i);

		if (ChildWidget != VisualGrid && ChildWidget.Get() != GraphPanel)
		{
			NativeWidgetVisibility.Add(ChildWidget.Get(), ChildWidget->GetVisibility());

			ChildWidget->SetVisibility(EVisibility::Hidden);
		}
	}
}

void UAccessibilityGraphLocomotionContext::UnHideNativeVisuals()
{
	FChildren* ViewportChildren = GridParent.Pin()->GetChildren();

	TSharedPtr<SWidget> ChildWidget;
	for (int32 i = 0; i < ViewportChildren->Num(); i++) 
	{
		ChildWidget = ViewportChildren->GetChildAt(i);
	
		if (NativeWidgetVisibility.Contains(ChildWidget.Get()))
		{
			ChildWidget->SetVisibility(NativeWidgetVisibility[ChildWidget.Get()]);
		}
    }

	NativeWidgetVisibility.Empty();
}

void UAccessibilityGraphLocomotionContext::OnFocusChanged(
	const FFocusEvent& FocusEvent,
	const FWeakWidgetPath& OldFocusedWidgetPath, const TSharedPtr<SWidget>& OldFocusedWidget,
    const FWidgetPath& NewFocusedWidgetPath, const TSharedPtr<SWidget>& NewFocusedWidget
) 
{
	if (!bIsActive)
		return;

	UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphLocomotion: FOCUS CHANGED."));

	TSharedPtr<SGraphEditor> LinkedEditorPtr = LinkedEditor.Pin();

	if (!NewFocusedWidgetPath.ContainsWidget(LinkedEditorPtr.ToSharedRef()))
	{
		bIsActive = false;
		Close();
	}
}

void UAccessibilityGraphLocomotionContext::BindFocusChangedEvent() 
{
	FocusChangedHandle = FSlateApplication::Get().OnFocusChanging()
		.AddUObject(this, &UAccessibilityGraphLocomotionContext::OnFocusChanged);
}

void UAccessibilityGraphLocomotionContext::UnbindFocusChangedEvent() 
{
	if (FocusChangedHandle.IsValid()) 
	{
		FSlateApplication::Get().OnFocusChanging().Remove(FocusChangedHandle);
    }
}
