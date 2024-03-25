// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseInputNode.h"
#include "InputContainers.h"

/**
 * 
 */
template<typename TEnum>
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseEnumInputNode : public FPhraseInputNode
{
public:
	FPhraseEnumInputNode(const TCHAR* NodeName);
	FPhraseEnumInputNode(const TCHAR* NodeName, TPhraseNodeArray InChildNodes);

	~FPhraseEnumInputNode();

protected:

	virtual bool MeetsInputRequirements(const FString& InPhrase) override;
	
	virtual bool RecordInput(const FString& InInput, FParseRecord& OutParseRecord) override;
};