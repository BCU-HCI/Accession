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

	UFUNCTION()
	static void KeyboardInputAdd(FParseRecord& Record);

	UFUNCTION()
	static void KeyboardInputRemove(FParseRecord& Record);

	UFUNCTION()
	static void KeyboardInputReset(FParseRecord& Record);

	UFUNCTION()
	static void KeyboardInputExit(FParseRecord& Record);

protected:

	static FString RemoveWordsFromEnd(const FString& InString, const int32& AmountToRemove);
};