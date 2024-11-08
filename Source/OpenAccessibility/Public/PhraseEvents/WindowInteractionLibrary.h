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
	 * Selects the Next Window in the Slate Application, and switches the Focus to it.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void SwitchNextActiveWindow(FParseRecord& Record);

	/**
	 * Selects the Previous Window in the Slate Application, and switches the Focus to it.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void SwitchPrevActiveWindow(FParseRecord& Record);

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


	// Window Tab Interaction

	/**
	 * Selects the Next Tab In The Active Tab Stack, and switches the Focus to it.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void SwitchNextTabInStack(FParseRecord& Record);

	/**
	 * Selects the Prev Tab In The Active Tab Stack, and switches the Focus to it.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void SwitchPrevTabInStack(FParseRecord& Record);

	/**
	 * Selects the Tab Based on User Input, and switches the Focus to it.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void SelectTabInStack(FParseRecord& Record);

	// End of Window Tab Interaction

protected:

	UPROPERTY(BlueprintReadOnly, Category = "WindowInteractionLibrary|ToolBar Accessibility")
	class UAccessibilityWindowToolbar* WindowToolBar;

};