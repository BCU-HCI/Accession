// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/PhraseNode.h"

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseInputNode : public FPhraseNode
{
public:
	FPhraseInputNode();
	~FPhraseInputNode();

	// FPhraseNode Implementation

	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord) override;

protected:

	/// <summary>
	/// The Phrase that be used to index the input into the record.
	/// </summary>
	FString InputPhrase;
};
