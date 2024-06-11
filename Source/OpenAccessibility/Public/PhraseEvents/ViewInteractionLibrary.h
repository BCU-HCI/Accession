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

	void BindBranches(TSharedRef<FPhraseTree> PhraseTree) override;

	// End of UPhraseTreeFunctionLibrary Implementation


	static void MoveViewport(FParseRecord& Record);

	static void ZoomViewport(FParseRecord& Record);

	static void IndexFocus(FParseRecord& Record);
};