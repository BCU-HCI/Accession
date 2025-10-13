// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/PhraseNode.h"

/**
 *
 */
class ACCESSIONCOMMUNICATION_API FPhraseEventNode : public FPhraseNode
{
public:
	FPhraseEventNode();
	FPhraseEventNode(TDelegate<void(FParseRecord &)> InEvent);
	FPhraseEventNode(TFunction<void(FParseRecord &)> InEventFunction);

	virtual ~FPhraseEventNode();

	// FPhraseNode Implementation
	virtual bool IsLeafNode() const override { return true; }

	virtual bool RequiresPhrase(const FString InPhrase) override;
	virtual bool RequiresPhrase(const FString InPhrase, int32 &OutDistance) override;

	virtual FParseResult ParsePhrase(TArray<FString> &InPhraseArray, FParseRecord &InParseRecord) override;
	// End FPhraseNode Implementation
};
