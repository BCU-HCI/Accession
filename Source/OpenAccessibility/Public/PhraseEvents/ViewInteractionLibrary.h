// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTreeFunctionLibrary.h"

#include "ViewInteractionLibrary.generated.h"

UCLASS()
class UViewInteractionLibrary : public UPhraseTreeFunctionLibrary
{
	GENERATED_BODY()

public:

	static void MoveViewport(FParseRecord& Record);

	static void ZoomViewport(FParseRecord& Record);

	static void IndexFocus(FParseRecord& Record);

};