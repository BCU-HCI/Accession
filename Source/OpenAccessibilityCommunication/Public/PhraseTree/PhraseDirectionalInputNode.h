#pragma once

#include "CoreMinimal.h"
#include "PhraseEnumInputNode.h"
#include "InputContainers.h"

class OPENACCESSIBILITYCOMMUNICATION_API FPhraseDirectionalInputNode : public FPhraseEnumInputNode<EPhraseDirectionalInput>
{
public:
	FPhraseDirectionalInputNode(const TCHAR* NodeName)
		: FPhraseEnumInputNode<EPhraseDirectionalInput>(NodeName)
	{}

	FPhraseDirectionalInputNode(const TCHAR* NodeName, TPhraseNodeArray InChildNodes)
		: FPhraseEnumInputNode<EPhraseDirectionalInput>(NodeName, InChildNodes)
	{}
};

class OPENACCESSIBILITYCOMMUNICATION_API FPhrase2DDirectionalInputNode : public FPhraseEnumInputNode<EPhrase2DDirectionalInput>
{
public:
	FPhrase2DDirectionalInputNode(const TCHAR* NodeName)
		: FPhraseEnumInputNode<EPhrase2DDirectionalInput>(NodeName)
	{}

	FPhrase2DDirectionalInputNode(const TCHAR* NodeName, TPhraseNodeArray InChildNodes)
		: FPhraseEnumInputNode<EPhrase2DDirectionalInput>(NodeName, InChildNodes)
	{}
};