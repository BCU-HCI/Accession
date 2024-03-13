// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseInputNode.h"

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseDirectionalInputNode : public FPhraseInputNode
{
public:
	FPhraseDirectionalInputNode(const TCHAR* InInputString);
	FPhraseDirectionalInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes);

	~FPhraseDirectionalInputNode();

protected:

	// FPhraseInputNode Implementation

	virtual bool MeetsInputRequirements(const FString& InPhrase) override;

	virtual bool RecordInput(const FString& InInput, FParseRecord& OutParseRecord) override;

	// End FPhraseInputNode Implementation

};
