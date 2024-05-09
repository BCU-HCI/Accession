// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/Containers/ContextObject.h"
#include "PhraseTree/Containers/ContextObject.h"

class IPhraseContextNodeBase
{
protected:
	virtual bool HasContextObject(TArray<UPhraseTreeContextObject*> InContextObjects) const = 0;

	virtual UPhraseTreeContextObject* CreateContextObject() = 0;

	virtual void ConstructContextChildren(TArray<TSharedPtr<class FPhraseNode>>& InChildNodes) = 0;
};