// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/IPhraseContextNode.h"
#include "PhraseTree/Containers/ContextObject.h"

template<class ContextType = UPhraseTreeContextObject>
class FPhraseContextNode : public FPhraseNode, public IPhraseContextNodeBase
{
public:

	FPhraseContextNode(const TCHAR* InInputString);
	FPhraseContextNode(const TCHAR* InInputString, TPhraseNodeArray InChildNodes);
	FPhraseContextNode(const TCHAR* InInputString, TDelegate<void(FParseRecord& Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes);

	~FPhraseContextNode();

	// FPhraseNode Implementation

	virtual FParseResult ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord) override;

	// End FPhraseNode Implementation

protected:

	// FPhraseContextNodeBase Implementation

	bool HasContextObject(TArray<UPhraseTreeContextObject*> InContextObjects) const;

	virtual UPhraseTreeContextObject* CreateContextObject();

	virtual void ConstructContextChildren(TPhraseNodeArray& InChildNodes);

	// End FPhraseContextNodeBase Implementation

};
