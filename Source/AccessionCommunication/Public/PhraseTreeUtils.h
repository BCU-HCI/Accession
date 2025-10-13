// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseTreeFunctionLibrary.h"

#include "PhraseTreeUtils.generated.h"

UCLASS()
class ACCESSIONCOMMUNICATION_API UPhraseTreeUtils : public UObject
{
	GENERATED_BODY()

public:
	UPhraseTreeUtils();

	virtual ~UPhraseTreeUtils();

	// Function Library Methods

	/**
	 * Registers the provided Phrase Tree Function Library.
	 * @param LibraryToRegister The Phrase Tree Function Library to Register.
	 */
	void RegisterFunctionLibrary(UPhraseTreeFunctionLibrary *LibraryToRegister);

	/**
	 * Sets the Phrase Tree Reference used for Registering Phrase Tree Function Libraries.
	 * @param NewPhraseTree Reference to the Phrase Tree to use.
	 */
	void SetPhraseTree(TSharedRef<FPhraseTree> NewPhraseTree)
	{
		this->PhraseTree = NewPhraseTree;
	}

protected:
	/**
	 * An Array of all the Registered Phrase Tree Function Libraries.
	 */
	UPROPERTY(EditAnywhere, Category = "PhraseTreeUtils")
	TArray<UPhraseTreeFunctionLibrary *> RegisteredLibraries;

	/**
	 * Weak Pointer to the Current Phrase Tree Instance used in Registering the Phrase Tree Function Libraries.
	 */
	TWeakPtr<FPhraseTree> PhraseTree;
};
