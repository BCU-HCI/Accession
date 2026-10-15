// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseTreeFunctionLibrary.h"

#include "ViewportInteractionLibrary.generated.h"

class FEditorViewportClient;

/**
 * Voice commands for 3D editor viewports.
 *
 * VIEW FOCUS <n> is bound by UNodeInteractionLibrary and dispatches here (via the navigator) when no graph editor is active,
 * as two identical phrase branches cannot coexist in the Phrase Tree.
 */
UCLASS()
class UViewportInteractionLibrary : public UPhraseTreeFunctionLibrary
{
	GENERATED_BODY()

public:

	void BindBranches(TSharedRef<FPhraseTree> PhraseTree) override;

	/** VIEW MOVE <direction> <amount> - Pans the camera relative to its facing. */
	void MoveViewport(FParseRecord& Record);

	/** VIEW NEXT - Frames the next numbered item. */
	void FocusNext(FParseRecord& Record);

	/** VIEW PREVIOUS - Frames the previous numbered item. */
	void FocusPrevious(FParseRecord& Record);

	/** VIEW BACK - Returns the camera to where it was before the last navigation step. */
	void CameraBack(FParseRecord& Record);

	/** VIEW FRAME - Frames the selected actors, or everything numbered when nothing is selected. */
	void FrameSelection(FParseRecord& Record);

	/** VIEW ORBIT <direction> <degrees> - Orbits around the item in the centre of view. */
	void OrbitViewport(FParseRecord& Record);

	/** VIEW NUMBERS - Shows or hides the item numbers. */
	void ToggleNumbers(FParseRecord& Record);

private:

	void PanCamera(FEditorViewportClient* ViewportClient, const FVector& Direction, float Amount);
};
