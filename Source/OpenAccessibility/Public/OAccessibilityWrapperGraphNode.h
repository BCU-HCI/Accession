// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SGraphNode.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

/**
 * 
 */
class OPENACCESSIBILITY_API OAccessibilityWrapperGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(OAccessibilityWrapperGraphNode) {}

	SLATE_END_ARGS()

	OAccessibilityWrapperGraphNode();
	~OAccessibilityWrapperGraphNode();

	virtual void Construct(const FArguments& InArgs, UEdGraphNode* InNode);

private:
	// Possible Wrapper Functionality
	// Like registering the node with a manager.
};
