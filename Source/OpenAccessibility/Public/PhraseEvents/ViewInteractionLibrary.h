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


	void MoveViewport(FParseRecord& Record);

	void ZoomViewport(FParseRecord& Record);

	void IndexFocus(FParseRecord& Record);
};