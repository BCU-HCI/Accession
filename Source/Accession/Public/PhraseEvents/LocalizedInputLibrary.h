// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseTreeFunctionLibrary.h"

#include "LocalizedInputLibrary.generated.h"

UCLASS()
class ULocalizedInputLibrary : public UPhraseTreeFunctionLibrary
{
	GENERATED_BODY()

public:
	ULocalizedInputLibrary(const FObjectInitializer &ObjectInitializer);

	virtual ~ULocalizedInputLibrary();

	// UPhraseTreeFunctionLibrary Implementation

	/**
	 * Binds Branches originating from this Library onto the provided Phrase Tree.
	 * @param PhraseTree Reference to the PhraseTree to Bind this Library to.
	 */
	virtual void BindBranches(TSharedRef<FPhraseTree> PhraseTree) override;

	// End of UPhraseTreeFunctionLibrary Implementation

	// Keyboard Input Implementation

	/**
	 * Phrase Event for Adding String Words to the Active Keyboard Focus.
	 * @param Record The ParseRecord accumulated until this Event.
	 */
	UFUNCTION()
	void KeyboardInputAdd(FParseRecord &Record);

	/**
	 * Phrase Event for Removing String Chunks from the Active Keyboard Focus.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void KeyboardInputRemove(FParseRecord &Record);

	/**
	 * Phrase Event for Resetting the Active Keyboard Focus.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void KeyboardInputReset(FParseRecord &Record);

	/**
	 * Phrase Event for Submitting the Keyboard Input on the Active Keyboard Focus.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void KeyboardInputConfirm(FParseRecord &Record);

	/**
	 * Phrase Event for Exiting the Active Keyboard Focus, with no changes.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void KeyboardInputExit(FParseRecord &Record);

	// End of Keyboard Input Implementation

	// Mouse Input Implementation

	// End of Keyboard Input Implementation
};