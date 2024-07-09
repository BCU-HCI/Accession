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
	FPhraseEventNode(TDelegate<void(FParseRecord&)> InEvent);
	FPhraseEventNode(TFunction<void(FParseRecord&)> InEventFunction);

	~FPhraseEventNode();

	// FPhraseNode Implementation
	virtual bool IsLeafNode() const override { return true; }

	virtual bool RequiresPhrase(const FString InPhrase) override;
	virtual bool RequiresPhrase(const FString InPhrase, int32& OutDistance);

	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseArray, FParseRecord& InParseRecord) override;
	// End FPhraseNode Implementation
};
