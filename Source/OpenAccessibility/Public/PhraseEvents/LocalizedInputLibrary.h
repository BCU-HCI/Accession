// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseTreeFunctionLibrary.h"

#include "LocalizedInputLibrary.generated.h"

UCLASS()
class ULocalizedInputLibrary : public UPhraseTreeFunctionLibrary 
{
	GENERATED_BODY()

public:

	ULocalizedInputLibrary(const FObjectInitializer& ObjectInitializer);

	virtual ~ULocalizedInputLibrary();

	// UPhraseTreeFunctionLibrary Implementation

	void BindBranches(TSharedRef<FPhraseTree> PhraseTree) override;

	// End of UPhraseTreeFunctionLibrary Implementation


	// Keyboard Input Implementation

	UFUNCTION()
	void KeyboardInputAdd(FParseRecord& Record);

	UFUNCTION()
	void KeyboardInputRemove(FParseRecord& Record);

	UFUNCTION()
	void KeyboardInputReset(FParseRecord& Record);

	UFUNCTION()
	void KeyboardInputExit(FParseRecord& Record);

	// End of Keyboard Input Implementation


	// Mouse Input Implementation



	// End of Keyboard Input Implementation

protected:

	//static FString RemoveWordsFromEnd(const FString& InString, const int32& AmountToRemove);
};