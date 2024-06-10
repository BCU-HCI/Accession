// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseTreeFunctionLibrary.h"

#include "PhraseTreeUtils.generated.h"

UCLASS()
class OPENACCESSIBILITYCOMMUNICATION_API UPhraseTreeUtils : public UObject
{
	GENERATED_BODY()

public:

	UPhraseTreeUtils();

	virtual ~UPhraseTreeUtils();

	// Function Library Methods

	void RegisterFunctionLibrary(const UPhraseTreeFunctionLibrary* LibraryToRegister);

	void SetPhraseTree(TSharedRef<FPhraseTree> NewPhraseTree)
	{
		this->PhraseTree = NewPhraseTree;
	}

protected:

	UPROPERTY(BlueprintReadOnly)
	TArray<UPhraseTreeFunctionLibrary*> RegisteredLibraries;


	TWeakPtr<FPhraseTree> PhraseTree;
};

