// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ContextObject.generated.h"

class FPhraseNode;

UCLASS(Abstract)
class OPENACCESSIBILITYCOMMUNICATION_API UPhraseTreeContextObject : public UObject
{
	GENERATED_BODY()

public:

	UPhraseTreeContextObject() 
		: UObject()
	{

	}

	~UPhraseTreeContextObject()
	{

	}

	
	void SetContextRootNode(TSharedRef<FPhraseNode> InRootNode)
	{
		ContextRoot = InRootNode;
	}

	TSharedPtr<FPhraseNode> GetContextRoot()
	{
		return ContextRoot.Pin();
	}

	const bool GetIsActive()
	{
		return bIsActive;
	}
protected:

	bool bIsActive = true;

	TWeakPtr<FPhraseNode> ContextRoot;
};