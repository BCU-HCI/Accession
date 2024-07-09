// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWrappers/AccessibilityGraphLocomotionContext.h"
#include "AccessibilityWidgets/SIndexer.h"
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

	Init(LinkedEditorPtr.ToSharedRef());
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

bool UAccessibilityGraphLocomotionContext::SelectChunk(int32 Index)
{
	if (Index > ChunkArray.Num() || Index < 0)
		return false;

	FGraphLocomotionChunk SelectedChunk = ChunkArray[Index];

	SGraphPanel* LinkedPanel = LinkedEditor.Pin()->GetGraphPanel();

	FVector2D TopLeftCoord = LinkedPanel->PanelCoordToGraphCoord(SelectedChunk.GetChunkTopLeft());
	FVector2D BottomRightCoord = LinkedPanel->PanelCoordToGraphCoord(SelectedChunk.GetChunkBottomRight());

	FPanelViewPosition PrevView = CurrentViewPosition;

	if (!MoveViewport(FPanelViewPosition(TopLeftCoord, BottomRightCoord)))
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("Failed To Jump To Viewport Coords (TopLeft: %s | BottomRight: %s)"), *TopLeftCoord.ToString(), *BottomRightCoord.ToString());
		return false;
	}

	if (PrevView != FVector2D::ZeroVector)
		PreviousPositions.Push(PrevView);

	return true;
}

bool UAccessibilityGraphLocomotionContext::RevertToPreviousView()
{
	if (PreviousPositions.IsEmpty())
	{
		LinkedEditor.Pin()->ZoomToFit(false);
		return true;
	}

	if (!MoveViewport(PreviousPositions.Pop()))
	{
		return false;
	}

	return true;
}

void UAccessibilityGraphLocomotionContext::ConfirmSelection()
{
	Close();
}

bool UAccessibilityGraphLocomotionContext::Close() 
{
	UnbindFocusChangedEvent();

	RemoveVisualGrid();
	UnHideNativeVisuals();

	bIsActive = false;

	RemoveFromRoot();
	MarkAsGarbage();

	UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphLocomotion: CONTEXT CLOSED."));

	return true;
}

bool UAccessibilityGraphLocomotionContext::MoveViewport(FPanelViewPosition NewViewPosition)
{
	if (!LinkedEditor.IsValid())
		return false;

	SGraphPanel* LinkedPanel = LinkedEditor.Pin()->GetGraphPanel();

	if (!LinkedPanel->JumpToRect(NewViewPosition.BotRight, NewViewPosition.TopLeft))
	{
		return false;
	}

	CurrentViewPosition = NewViewPosition;

	return true;
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
	ChunkArray.Reset(InVisualChunkSize.X * InVisualChunkSize.Y);
	ChunkSize = InVisualChunkSize;

	TSharedPtr<SUniformGridPanel> GridContainerPtr = GridContainer.Pin();

	int32 ChunkIndex = -1;
	TSharedPtr<SBox> ChunkWidget;
	TSharedPtr<SIndexer> ChunkIndexer;

	for (int32 Y = 0; Y < InVisualChunkSize.Y; Y++)
	{
		for (int32 X = 0; X < InVisualChunkSize.X; X++)
		{
			ChunkIndex = X + (Y * InVisualChunkSize.X);
			FGraphLocomotionChunk& GraphChunk = ChunkArray.EmplaceAt_GetRef(ChunkIndex);

			GridContainerPtr->AddSlot(X, Y)
			[
				SAssignNew(ChunkWidget, SBox)
				[
					SNew(SBorder)
					.Padding(0.5f)
					.BorderBackgroundColor(FLinearColor(0.f, .75f, 0.f))
					[
						SNew(SBorder)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.BorderBackgroundColor(FLinearColor::Green)
						[
							SAssignNew(ChunkIndexer, SIndexer)
							.TextColor(FLinearColor::Green)
							.IndexValue(ChunkIndex)
						]
					]
				]
			];

			GraphChunk.ChunkWidget = ChunkWidget;
			GraphChunk.ChunkIndexer = ChunkIndexer;
		}
	}

	CalculateVisualChunksBounds();
}

void UAccessibilityGraphLocomotionContext::CalculateVisualChunksBounds()
{
	if (!LinkedEditor.IsValid())
		return;

	SGraphPanel* LinkedPanel = LinkedEditor.Pin()->GetGraphPanel();
	FVector2D PanelGeoSize = LinkedPanel->GetTickSpaceGeometry().GetLocalSize();

	double ChunkWidgetSizeX = PanelGeoSize.X / ChunkSize.X;
	double ChunkWidgetSizeY = PanelGeoSize.Y / ChunkSize.Y;

	FGraphLocomotionChunk Chunk;
	double ChunkX, ChunkY;

	int32 ArrIndex;
	for (int Y = 0; Y < ChunkSize.Y; Y++)
	{
		for (int X = 0; X < ChunkSize.X; X++)
		{
			ArrIndex = (Y * ChunkSize.X) + X;

			Chunk = ChunkArray[ArrIndex];

			ChunkX = X * ChunkWidgetSizeX;
			ChunkY = Y * ChunkWidgetSizeY;

			Chunk.SetChunkBounds(
				FVector2D(ChunkX, ChunkY), 
				FVector2D(ChunkWidgetSizeX + ChunkX, ChunkWidgetSizeY + ChunkY)
			);

			ChunkArray[ArrIndex] = Chunk;
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
