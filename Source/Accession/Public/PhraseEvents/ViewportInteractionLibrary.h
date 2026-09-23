// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseTreeFunctionLibrary.h"

#include "ViewportInteractionLibrary.generated.h"

UCLASS()
class UViewportInteractionLibrary : public UPhraseTreeFunctionLibrary
{
	GENERATED_BODY()

public:

	void BindBranches(TSharedRef<FPhraseTree> PhraseTree) override;

	void MoveViewport(FParseRecord& Record);
	void ZoomViewport(FParseRecord& Record);

private:

	void PanCamera(FEditorViewportClient* ViewportClient, const FVector2D& Direction, float Amount);

	static FEditorViewportClient* GetViewportClient(const TSharedPtr<SWidget>& Widget);
};