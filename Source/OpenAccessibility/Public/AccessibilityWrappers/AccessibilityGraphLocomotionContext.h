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
  /// Weak Pointer to the Chunks Indexer Widget.
  /// </summary>
  TWeakPtr<class SIndexer> ChunkIndexer;

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

	bool SelectChunk(int32 Index);

	bool RevertToPreviousView();

	void ConfirmSelection();

	void Close();

protected:

	void CreateVisualGrid(TSharedRef<SGraphEditor> InGraphEditor);

	void GenerateVisualChunks(TSharedRef<SGraphEditor> InGraphEditor, FIntVector2 InVisualChunkSize = FIntVector2(10));

	void CalculateVisualChunksBounds();

	void RemoveVisualGrid();

	void HideNativeVisuals();

	void UnHideNativeVisuals();

	void OnFocusChanged(const FFocusEvent& FocusEvent, const FWeakWidgetPath& OldFocusedWidgetPath, const TSharedPtr<SWidget>& OldFocusedWidget, const FWidgetPath& NewFocusedWidgetPath, const TSharedPtr<SWidget>& NewFocusedWidget);


	void BindFocusChangedEvent();

	void UnbindFocusChangedEvent();

protected:

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

		FVector2D TopLeft;
		FVector2D BotRight;
	};
	FPanelViewPosition DefaultViewPosition;
	TArray<FPanelViewPosition> PreviousPositions;

	// Chunking References

	TArray<FGraphLocomotionChunk> ChunkArray;

	FIntVector2 ChunkSize;


	// Container References

	TWeakPtr<SUniformGridPanel> GridContainer;

	TWeakPtr<SOverlay> GridParent;

	TWeakPtr<SGraphEditor> LinkedEditor;

private:

	TMap<SWidget*, EVisibility> NativeWidgetVisibility;

	FDelegateHandle FocusChangedHandle;
};