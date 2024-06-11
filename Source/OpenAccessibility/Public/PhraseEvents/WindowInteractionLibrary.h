// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseTreeFunctionLibrary.h"

#include "WindowInteractionLibrary.generated.h"

UCLASS()
class UWindowInteractionLibrary : public UPhraseTreeFunctionLibrary
{
	GENERATED_BODY()

public:

	UWindowInteractionLibrary(const FObjectInitializer& ObjectInitializer);

	virtual ~UWindowInteractionLibrary();

	// UPhraseTreeFunctionLibrary Implementation

	void BindBranches(TSharedRef<FPhraseTree> PhraseTree) override;

	// End of UPhraseTreeFunctionLibrary Implementation


	// Window Interaction

	void CloseActiveWindow(FParseRecord& Record);

	// End Window Interaction


	// Window ToolBar Interaction

	void SelectToolBarItem(FParseRecord& Record);

	// End Window ToolBar Interaction

};