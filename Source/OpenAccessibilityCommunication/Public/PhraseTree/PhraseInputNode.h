// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/PhraseNode.h"

/**
 * 
 */
template <typename InputType = int32>
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseInputNode : public FPhraseNode
{
public:
	FPhraseInputNode(const TCHAR* InInputString);
	FPhraseInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes);
	FPhraseInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes);
	FPhraseInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void (InputType Input)> InOnInputRecieved);
	FPhraseInputNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(InputType Input)> InOnInputRecieved);

	~FPhraseInputNode();

	// FPhraseNode Implementation
	
	virtual bool RequiresPhrase(const FString InPhrase) override;

	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord) override;

	// End FPhraseNode Implementation

	TDelegate<void(InputType ReceivedInput)> OnInputReceived;

protected:

	virtual bool MeetsInputRequirements(const FString& InPhrase);

	virtual bool RecordInput(const FString& InInput, FParseRecord& OutParseRecord);
};
