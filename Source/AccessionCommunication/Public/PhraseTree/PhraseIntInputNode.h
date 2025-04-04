// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/PhraseInputNode.h"
#include "AccessionComLogging.h"

#include "PhraseTree/Containers/Input/UParseIntInput.h"

/**
 *
 */
class ACCESSIONCOMMUNICATION_API FPhraseIntInputNode : public FPhraseInputNode<int32>
{
public:
	FPhraseIntInputNode(const TCHAR *InInputString);
	FPhraseIntInputNode(const TCHAR *InInputString, TPhraseNodeArray InChildNodes);
	FPhraseIntInputNode(const TCHAR *InInputString, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes);
	FPhraseIntInputNode(const TCHAR *InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved);
	FPhraseIntInputNode(const TCHAR *InInputString, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved);

	virtual ~FPhraseIntInputNode() override;

protected:
	/// <summary>
	/// Checks if the Given Phrase Meets Requirements for usage as Input.
	/// In Correlation to this Nodes Input Specifications.
	/// </summary>
	/// <param name="InPhrase">- The Phrase To Check If It Meets Requirements.</param>
	/// <returns>True, if the Phrase Meets Requirements. Otherwise False.</returns>
	virtual bool MeetsInputRequirements(const FString &InPhrase) override;

	/// <summary>
	/// Records the Input onto the Parse Record.
	/// </summary>
	/// <param name="InInput">- The Phrase To Record onto the Parse Record.</param>
	/// <param name="OutParseRecord">- Returns the Updated ParseRecord.</param>
	/// <returns>True, if the Input Was Successful in Recording. Otherwise False.</returns>
	virtual bool RecordInput(const FString &InInput, FParseRecord &OutParseRecord) override;
};
