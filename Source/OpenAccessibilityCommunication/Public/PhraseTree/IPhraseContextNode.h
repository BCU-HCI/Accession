// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/Containers/ContextObject.h"
#include "PhraseTree/Containers/ContextObject.h"

/// <summary>
/// Base Abstract Class For Phrase Context Nodes, that are required to have a Context Node.
/// </summary>
class IPhraseContextNodeBase
{
protected:

	/// <summary>
	/// Checks if the Given Context Array Contains Context Objects.
	/// </summary>
	/// <param name="InContextObjects">- The Array To Check For Context Objects.</param>
	/// <returns>True, if their is Context Objects in the Given Array.</returns>
	virtual bool HasContextObject(TArray<UPhraseTreeContextObject*> InContextObjects) const = 0;

	/// <summary>
	/// Creates a Context Object.
	/// </summary>
	/// <returns></returns>
	virtual UPhraseTreeContextObject* CreateContextObject() = 0;

	/// <summary>
	/// Constructs the Context Nodes Children, from Given Child Nodes.
	/// Allowing for Inclusion of Utility Nodes in relation to the Context.
	/// </summary>
	/// <param name="InChildNodes">- An Array of the Nodes Children.</param>
	virtual void ConstructContextChildren(TArray<TSharedPtr<class FPhraseNode>>& InChildNodes) = 0;
};