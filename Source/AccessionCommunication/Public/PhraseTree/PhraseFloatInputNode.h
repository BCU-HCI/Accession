// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"
#include "PhraseInputNode.h"
#include "AccessionComLogging.h"

#include "PhraseTree/Containers/Input/UParseFloatInput.h"

/**
 *
 */
class ACCESSIONCOMMUNICATION_API FPhraseFloatInputNode : public FPhraseInputNode<float>
{
public:
	FPhraseFloatInputNode(const TCHAR *InInputString);
	FPhraseFloatInputNode(const TCHAR *InInputString, TPhraseNodeArray InChildNodes);
	FPhraseFloatInputNode(const TCHAR *InInputString, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes);
	FPhraseFloatInputNode(const TCHAR *InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(float Input)> InOnInputRecieved);
	FPhraseFloatInputNode(const TCHAR *InInputString, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(float Input)> InOnInputRecieved);

	virtual ~FPhraseFloatInputNode() override;

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
