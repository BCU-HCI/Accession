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
	FPhraseInputNode(const TCHAR* InInputString);
	FPhraseInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes);

	~FPhraseInputNode();

	// FPhraseNode Implementation
	
	virtual bool RequiresPhrase(const FString InPhrase) override;

	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord) override;

	// End FPhraseNode Implementation
};
