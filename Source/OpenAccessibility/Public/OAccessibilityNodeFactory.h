// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

/**
 * 
 */
class OPENACCESSIBILITY_API FAccessibilityNodeFactory : public FGraphPanelNodeFactory
{

public:
	/* Begin FGraphPanelNodeFactory */
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
	/* End FGraphPanelNodeFactory */

public:
	FAccessibilityNodeFactory();
	~FAccessibilityNodeFactory();

	inline void WrapNodeWidget(UEdGraphNode* Node, TSharedRef<SGraphNode> NodeWidget, int NodeIndex) const;

	inline void WrapPinWidget(UEdGraphPin* Pin, TSharedRef<SGraphPin> PinWidget, int PinIndex, SGraphNode* OwnerNode) const;

	void SetSharedPtr(TSharedPtr<FAccessibilityNodeFactory> InSharedPtr)
	{
		ThisPtr = InSharedPtr;
	}

private:

	TSharedPtr<FAccessibilityNodeFactory> ThisPtr;
};
