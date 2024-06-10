// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTreeFunctionLibrary.h"

#include "WindowInteractionLibrary.generated.h"

UCLASS()
class UWindowInteractionLibrary : public UPhraseTreeFunctionLibrary
{
	GENERATED_BODY()

public:

	// Window Interaction

	static void CloseActiveWindow(FParseRecord& Record);

	// End Window Interaction


	// Window ToolBar Interaction

	static void SelectToolBarItem(FParseRecord& Record);

	// End Window ToolBar Interaction

};