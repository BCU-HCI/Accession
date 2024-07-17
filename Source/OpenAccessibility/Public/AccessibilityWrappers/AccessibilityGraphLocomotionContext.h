// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/Containers/ContextObject.h"

#include "Widgets/Layout/SUniformGridPanel.h"

#include "AccessibilityGraphLocomotionContext.generated.h"

USTRUCT()
struct FGraphLocomotionChunk
{
	GENERATED_BODY()

public:

	void SetChunkBounds(FVector2D InTopLeft, FVector2D InBottomRight)
	{
		TopLeft = InTopLeft;
		BottomRight = InBottomRight;
	}

	void GetChunkBounds(FVector2D& OutTopLeft, FVector2D& OutBottomRight) const
	{
		OutTopLeft = TopLeft;
		OutBottomRight = BottomRight;
	}

	FVector2D GetChunkTopLeft() const { return TopLeft; }

	FVector2D GetChunkBottomRight() const { return BottomRight; }

	void SetVisColor(const FLinearColor& NewColor) const
	{
		if (ChunkVisWidget.IsValid())
			ChunkVisWidget.Pin()->SetBorderBackgroundColor(NewColor);
	}

public:

	/// <summary>
	/// Visual Chunks Top Left Corner.
	/// </summary>
	FVector2D TopLeft;

	/// <summary>
	/// Visual Chunks Bottom Right Corner.
	/// </summary>
	FVector2D BottomRight;

	/// <summary>
	/// Weak Pointer to the Chunks Visual Widget.
	/// </summary>
	TWeakPtr<SBox> ChunkWidget;

	/// <summary>
	/// Weak Pointer to the Chunks Visual Widget.
	/// </summary>
	TWeakPtr<SBorder> ChunkVisWidget;

	/// <summary>
	/// Weak Pointer to the Chunks Indexer Widget.
	/// </summary>
	TWeakPtr<class SIndexer> ChunkIndexer;

};

struct FPanelViewPosition
{
public:

	FPanelViewPosition()
		: TopLeft(FVector2D::ZeroVector)
		, BotRight(FVector2D::ZeroVector)
	{ }

	FPanelViewPosition(FVector2D InTopLeft, FVector2D InBotRight)
		: TopLeft(InTopLeft)
		, BotRight(InBotRight)
	{ }

	bool operator!=(const FVector2D& Other)
	{
		return TopLeft != Other || BotRight != Other;
	}

	bool operator!=(const FPanelViewPosition& Other)
	{
		return TopLeft != Other.TopLeft || BotRight != Other.BotRight;
	}

	FVector2D TopLeft;
	FVector2D BotRight;
};

UCLASS()
class OPENACCESSIBILITY_API UAccessibilityGraphLocomotionContext : public UPhraseTreeContextObject
{
	GENERATED_BODY()

public:

	UAccessibilityGraphLocomotionContext(const FObjectInitializer& ObjectInitializer);
	
	virtual ~UAccessibilityGraphLocomotionContext();

	void Init();
	void Init(TSharedRef<SGraphEditor> InGraphEditor);

	bool SelectChunk(const int32& Index);

	bool RevertToPreviousView();

	void ConfirmSelection();

	void CancelLocomotion();

	virtual bool Close() override;

protected:

	bool MoveViewport(const FVector2D& InTopLeft, const FVector2D& InBottomRight) const;

	bool MoveViewport(const FPanelViewPosition& NewViewPosition) const;

	// Visuals Methods

	void ChangeChunkVis(const int32& Index, const FLinearColor& NewColor = FLinearColor::Yellow);

	void CreateVisualGrid(const TSharedRef<SGraphEditor> InGraphEditor);

	void GenerateVisualChunks(const TSharedRef<SGraphEditor> InGraphEditor, FIntVector2 InVisualChunkSize = FIntVector2(10));

	void CalculateVisualChunksBounds();

	void RemoveVisualGrid();

	void HideNativeVisuals();

	void UnHideNativeVisuals();

	void OnFocusChanged(const FFocusEvent& FocusEvent, const FWeakWidgetPath& OldFocusedWidgetPath, const TSharedPtr<SWidget>& OldFocusedWidget, const FWidgetPath& NewFocusedWidgetPath, const TSharedPtr<SWidget>& NewFocusedWidget);


	void BindFocusChangedEvent();

	void UnbindFocusChangedEvent();

protected:

	FVector2D StartViewPosition; float StartViewZoom;

	FPanelViewPosition CurrentViewPosition;
	TArray<FPanelViewPosition> PreviousPositions;

	// Chunking References

	TArray<FGraphLocomotionChunk> ChunkArray;

	FIntVector2 ChunkSize;


	// Container References

	TWeakPtr<SUniformGridPanel> GridContainer;

	TWeakPtr<SOverlay> GridParent;

	TWeakPtr<SGraphEditor> LinkedEditor;

private:

	FTimerHandle SelectionTimerHandle;

	TMap<SWidget*, EVisibility> NativeWidgetVisibility;

	FDelegateHandle FocusChangedHandle;
};