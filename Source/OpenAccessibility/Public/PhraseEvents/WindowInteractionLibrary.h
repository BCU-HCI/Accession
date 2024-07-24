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

	/**
	 * Binds Branches originating from this Library onto the provided Phrase Tree.
	 * @param PhraseTree Reference to the PhraseTree to Bind this Library to.
	 */
	void BindBranches(TSharedRef<FPhraseTree> PhraseTree) override;

	// End of UPhraseTreeFunctionLibrary Implementation


	// Window Interaction

	/**
	 * Closes the Top Most Active Window, if it is not the Root Application Window.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void CloseActiveWindow(FParseRecord& Record);

	// End Window Interaction


	// Window ToolBar Interaction

	/**
	 * Selects the Item from the Active Windows ToolBar.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void SelectToolBarItem(FParseRecord& Record);

	// End Window ToolBar Interaction


protected:

	UPROPERTY(BlueprintReadOnly)
	class UAccessibilityWindowToolbar* WindowToolBar;

};