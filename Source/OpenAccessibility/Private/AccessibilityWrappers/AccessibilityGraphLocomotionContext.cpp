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

void UAccessibilityGraphLocomotionContext::Init(const TSharedRef<SGraphEditor> InGraphEditor) 
{
    LinkedEditor = InGraphEditor;

	InGraphEditor->GetViewLocation(StartViewPosition, StartViewZoom);
    InGraphEditor->ZoomToFit(false);

    CreateVisualGrid(InGraphEditor);
    GenerateVisualChunks(InGraphEditor, FIntVector2(6, 4));

	HideNativeVisuals();

	BindFocusChangedEvent();
}

bool UAccessibilityGraphLocomotionContext::SelectChunk(const int32& Index)
{
	if (Index > ChunkArray.Num() || Index < 0)
		return false;

	const FGraphLocomotionChunk SelectedChunk = ChunkArray[Index];

	const SGraphPanel* LinkedPanel = LinkedEditor.Pin()->GetGraphPanel();

	const FVector2D GraphTopLeftCoord = LinkedPanel->PanelCoordToGraphCoord(SelectedChunk.GetChunkTopLeft());
	const FVector2D GraphBottomRightCoord = LinkedPanel->PanelCoordToGraphCoord(SelectedChunk.GetChunkBottomRight());

	ChangeChunkVis(Index, FLinearColor::Red);

	GEditor->GetTimerManager()->SetTimer(
		SelectionTimerHandle,
		[this, Index, GraphTopLeftCoord, GraphBottomRightCoord]()
		{
			ChangeChunkVis(Index);

			if (MoveViewport(GraphTopLeftCoord, GraphBottomRightCoord))
			{
				if (CurrentViewPosition != FVector2D::ZeroVector)
					PreviousPositions.Push(CurrentViewPosition);

				CurrentViewPosition = FPanelViewPosition(GraphTopLeftCoord, GraphBottomRightCoord);
			}
			else
			{
				UE_LOG(LogOpenAccessibility, Log, TEXT("Failed To Jump To Viewport Coords (TopLeft: %s | BottomRight: %s)"), 
					*GraphTopLeftCoord.ToString(), *GraphBottomRightCoord.ToString());
			}
		},
		0.5f,
		false
	);

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

void UAccessibilityGraphLocomotionContext::CancelLocomotion()
{
	if (LinkedEditor.IsValid())
	{
		LinkedEditor.Pin()->SetViewLocation(StartViewPosition, StartViewZoom);

		Close();
	}
}

bool UAccessibilityGraphLocomotionContext::Close() 
{
	UnbindFocusChangedEvent();

	if (SelectionTimerHandle.IsValid())
		GEditor->GetTimerManager()->ClearTimer(SelectionTimerHandle);

	RemoveVisualGrid();
	UnHideNativeVisuals();

	bIsActive = false;

	RemoveFromRoot();
	MarkAsGarbage();

	UE_LOG(LogOpenAccessibility, Warning, TEXT("GraphLocomotion: CONTEXT CLOSED."));

	return true;
}

bool UAccessibilityGraphLocomotionContext::MoveViewport(const FVector2D& InTopLeft, const FVector2D& InBottomRight) const
{
	if (!LinkedEditor.IsValid())
		return false;

	TSharedPtr<SGraphEditor> LinkedEditorPtr = LinkedEditor.Pin();
	SGraphPanel* LinkedPanel = LinkedEditorPtr->GetGraphPanel();

	return LinkedPanel->JumpToRect(InTopLeft, InBottomRight);
}

bool UAccessibilityGraphLocomotionContext::MoveViewport(const FPanelViewPosition& NewViewPosition) const
{
	if (!LinkedEditor.IsValid())
		return false;

	SGraphPanel* LinkedPanel = LinkedEditor.Pin()->GetGraphPanel();

	return LinkedPanel->JumpToRect(NewViewPosition.TopLeft, NewViewPosition.BotRight);
}

void UAccessibilityGraphLocomotionContext::ChangeChunkVis(const int32& Index, const FLinearColor& NewColor)
{
	check(Index < ChunkArray.Num() && Index >= 0)

	ChunkArray[Index].SetVisColor(NewColor);
}

void UAccessibilityGraphLocomotionContext::CreateVisualGrid(const TSharedRef<SGraphEditor> InGraphEditor)
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

void UAccessibilityGraphLocomotionContext::GenerateVisualChunks(const TSharedRef<SGraphEditor> InGraphEditor, FIntVector2 InVisualChunkSize) 
{
	ChunkArray.Reset(InVisualChunkSize.X * InVisualChunkSize.Y);
	ChunkSize = InVisualChunkSize;

	TSharedPtr<SUniformGridPanel> GridContainerPtr = GridContainer.Pin();

	int32 ChunkIndex = -1;
	TSharedPtr<SBox> ChunkWidget;
	TSharedPtr<SBorder> ChunkVisWidget;
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
					SAssignNew(ChunkVisWidget, SBorder)
					.Padding(0.5f)
					.BorderBackgroundColor(FLinearColor::Yellow)
					[
						SNew(SBorder)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.BorderBackgroundColor(FLinearColor::Yellow)
						[
							SAssignNew(ChunkIndexer, SIndexer)
							.TextColor(FLinearColor::Yellow)
							.IndexValue(ChunkIndex)
						]
					]
				]
			];

			GraphChunk.ChunkWidget = ChunkWidget;
			GraphChunk.ChunkVisWidget = ChunkVisWidget;
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

	if (!NewFocusedWidgetPath.ContainsWidget(LinkedEditorPtr.Get()))
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
