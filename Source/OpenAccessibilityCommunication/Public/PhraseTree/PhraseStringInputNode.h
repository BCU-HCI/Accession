// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseInputNode.h"

/**
 *
 */
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseStringInputNode : public FPhraseInputNode<FString>
{
	FPhraseStringInputNode(const TCHAR* InInputString);
	FPhraseStringInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes);
	FPhraseStringInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes);
	FPhraseStringInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(FString Input)> InOnInputRecieved);
	FPhraseStringInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(FString Input)> InOnInputRecieved);

	~FPhraseStringInputNode();

protected:

	// FPhraseInputNode Implementation

	virtual bool MeetsInputRequirements(const FString& InPhrase) override;

	virtual bool RecordInput(const FString& InInput, FParseRecord& OutParseRecord) override;

	// End FPhraseInputNode Implementation
};