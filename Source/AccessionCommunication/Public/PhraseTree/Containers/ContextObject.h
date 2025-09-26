// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"

#include "ContextObject.generated.h"

class FPhraseNode;

UCLASS(Abstract)
class ACCESSIONCOMMUNICATION_API UPhraseTreeContextObject : public UObject
{
	GENERATED_BODY()

public:
	UPhraseTreeContextObject()
		: UObject()
	{
	}

	virtual ~UPhraseTreeContextObject()
	{
	}

	virtual bool Close() { return true; }

	/// <summary>
	/// Sets the Root Node In The Phrase Tree For This Context Objects.
	/// </summary>
	/// <param name="InRootNode"></param>
	void SetContextRootNode(TSharedRef<FPhraseNode> InRootNode)
	{
		ContextRoot = InRootNode;
	}

	/// <summary>
	/// Gets the Root Node For This Context.
	/// </summary>
	/// <returns></returns>
	TSharedPtr<FPhraseNode> GetContextRoot()
	{
		return ContextRoot.Pin();
	}

	/// <summary>
	/// Is the Context Still Active.
	/// </summary>
	/// <returns></returns>
	const bool GetIsActive()
	{
		return bIsActive;
	}

protected:
	/// <summary>
	/// Is the Context Object Still Active.
	/// </summary>
	bool bIsActive = true;

	/// <summary>
	/// The Root Node In The Phrase Tree (The Origin of the Context).
	/// Allowing for Propagation based on Context Root.
	/// </summary>
	TWeakPtr<FPhraseNode> ContextRoot;
};