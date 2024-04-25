// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseInputNode.h"
#include "Containers/Input/InputContainers.h"

/**
 * 
 */
template<typename TEnum>
class OPENACCESSIBILITYCOMMUNICATION_API FPhraseEnumInputNode : public FPhraseInputNode
{
public:
	FPhraseEnumInputNode(const TCHAR* InInputString);
	FPhraseEnumInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes);
	FPhraseEnumInputNode(const TCHAR* InInputString, TDelegate<void(const FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes);
	FPhraseEnumInputNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved);
	FPhraseEnumInputNode(const TCHAR* InInputString, TDelegate<void(const FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved);

	~FPhraseEnumInputNode();

protected:

	virtual bool MeetsInputRequirements(const FString& InPhrase) override;
	
	virtual bool RecordInput(const FString& InInput, FParseRecord& OutParseRecord) override;
};