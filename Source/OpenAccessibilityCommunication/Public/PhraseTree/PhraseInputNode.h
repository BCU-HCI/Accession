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

	virtual bool RequiresPhrase(const FString InPhrase, int32& OutDistance) override;

	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord) override;

	// End FPhraseNode Implementation

	TDelegate<void(InputType ReceivedInput)> OnInputReceived;

protected:

	/// <summary>
	/// Checks if the Given Phrase Meets Requirements for usage as Input.
	/// In Correlation to this Nodes Input Specifications.
	/// </summary>
	/// <param name="InPhrase">- The Phrase To Check If It Meets Requirements.</param>
	/// <returns>True, if the Phrase Meets Requirements. Otherwise False.</returns>
	virtual bool MeetsInputRequirements(const FString& InPhrase);

	/// <summary>
	/// Records the Input onto the Parse Record.
	/// </summary>
	/// <param name="InInput">- The Phrase To Record onto the Parse Record.</param>
	/// <param name="OutParseRecord">- Returns the Updated ParseRecord.</param>
	/// <returns>True, if the Input Was Successful in Recording. Otherwise False.</returns>
	virtual bool RecordInput(const FString& InInput, FParseRecord& OutParseRecord);
};
