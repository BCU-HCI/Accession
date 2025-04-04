// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseTreeFunctionLibrary.h"

#include "ViewInteractionLibrary.generated.h"

UCLASS()
class UViewInteractionLibrary : public UPhraseTreeFunctionLibrary
{
	GENERATED_BODY()

public:

	UViewInteractionLibrary(const FObjectInitializer& ObjectInitializer);

	virtual ~UViewInteractionLibrary();

	// UPhraseTreeFunctionLibrary Implementation

	/**
	 * Binds Branches originating from this Library onto the provided Phrase Tree.
	 * @param PhraseTree Reference to the PhraseTree to Bind this Library to.
	 */
	void BindBranches(TSharedRef<FPhraseTree> PhraseTree) override;

	// End of UPhraseTreeFunctionLibrary Implementation


	/**
	 * Phrase Event for Moving the Active Viewport.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void MoveViewport(FParseRecord& Record);

	/**
	 * Phrase Event for Zooming the Active Viewport.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void ZoomViewport(FParseRecord& Record);

	/**
	 * Phrase Event for Focusing on the Active Viewports Indexed Item, if one is apparent.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void IndexFocus(FParseRecord& Record);
};