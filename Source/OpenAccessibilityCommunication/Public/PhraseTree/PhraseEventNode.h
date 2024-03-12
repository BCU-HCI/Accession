// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/PhraseNode.h"

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseEventNode : public FPhraseNode
{
public:
	FPhraseEventNode();
	FPhraseEventNode(TDelegate<void(const FParseRecord&)> InEvent);

	~FPhraseEventNode();

	// FPhraseNode Implementation
	virtual bool IsLeafNode() const override { return true; }
	virtual bool RequiresPhrase(const FString InPhrase) override;

	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord) override;
	// End FPhraseNode Implementation


	TDelegate<void(const FParseRecord&)> OnPhraseEvent;
};
