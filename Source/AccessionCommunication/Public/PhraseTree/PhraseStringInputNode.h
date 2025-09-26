// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"
#include "PhraseInputNode.h"

/**
 *
 */
class ACCESSIONCOMMUNICATION_API FPhraseStringInputNode : public FPhraseInputNode<FString>
{
public:
	FPhraseStringInputNode(const TCHAR *InInputString);
	FPhraseStringInputNode(const TCHAR *InInputString, TPhraseNodeArray InChildNodes);
	FPhraseStringInputNode(const TCHAR *InInputString, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes);
	FPhraseStringInputNode(const TCHAR *InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(FString Input)> InOnInputRecieved);
	FPhraseStringInputNode(const TCHAR *InInputString, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(FString Input)> InOnInputRecieved);

	virtual ~FPhraseStringInputNode();

protected:
	// FPhraseInputNode Implementation

	virtual bool MeetsInputRequirements(const FString &InPhrase) override;

	virtual bool RecordInput(const FString &InInput, FParseRecord &OutParseRecord) override;

	// End FPhraseInputNode Implementation
};