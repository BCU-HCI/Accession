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
	
	void SetChunkBounds(FVector2f InTopLeft, FVector2f InBottomRight) 
	{
		TopLeft = InTopLeft;
		BottomRight = InBottomRight;
	}

	void GetChunkBounds(FVector2f& OutTopLeft, FVector2f& OutBottomRight) const
	{
		OutTopLeft = TopLeft;
        OutBottomRight = BottomRight;
    }

	FVector2f GetChunkTopLeft() const { return TopLeft; } 

	FVector2f GetChunkBottomRight() const { return BottomRight; }

public:

  /// <summary>
  /// Visual Chunks Top Left Corner.
  /// </summary>
  FVector2f TopLeft;

  /// <summary>
  /// Visual Chunks Bottom Right Corner.
  /// </summary>
  FVector2f BottomRight;

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

	void Close();

protected:

	void CreateVisualGrid(TSharedRef<SGraphEditor> InGraphEditor);

	void GenerateVisualChunks(TSharedRef<SGraphEditor> InGraphEditor, FIntVector2 InVisualChunkSize = FIntVector2(10));

	void RemoveVisualGrid();

	void HideNativeVisuals();

	void UnHideNativeVisuals();

	void OnFocusChanged(const FFocusEvent& FocusEvent, const FWeakWidgetPath& OldFocusedWidgetPath, const TSharedPtr<SWidget>& OldFocusedWidget, const FWidgetPath& NewFocusedWidgetPath, const TSharedPtr<SWidget>& NewFocusedWidget);


	void BindFocusChangedEvent();

	void UnbindFocusChangedEvent();

protected:

	TArray<FGraphLocomotionChunk> ChunkArray;

	TWeakPtr<SUniformGridPanel> GridContainer;

	TWeakPtr<SOverlay> GridParent;

	TWeakPtr<SGraphEditor> LinkedEditor;

private:

	TMap<SWidget*, EVisibility> NativeWidgetVisibility;

	FDelegateHandle FocusChangedHandle;
};