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

	/// <summary>
	/// Wraps the Node Widget with Accessibility Indexing.
	/// </summary>
	/// <param name="Node">The Node Object That is Being Wrapped.</param>
	/// <param name="NodeWidget">The Node Widget That is Being Wrapped.</param>
	/// <param name="NodeIndex">The Index of the Node.</param>
	inline void WrapNodeWidget(UEdGraphNode* Node, TSharedRef<SGraphNode> NodeWidget, int NodeIndex) const;

	/// <summary>
	/// Wraps the Pin Widget with Accessibility Indexing.
	/// </summary>
	/// <param name="Pin">The Pin Object That is Being Wrapped.</param>
	/// <param name="PinWidget">The Node Widget That is Being Wrapped.</param>
	/// <param name="PinIndex">The Index of the Pin.</param>
	/// <param name="OwnerNode">The Owning Node Widget of the Pin.</param>
	inline void WrapPinWidget(UEdGraphPin* Pin, TSharedRef<SGraphPin> PinWidget, int PinIndex, SGraphNode* OwnerNode) const;

	void SetSharedPtr(TSharedPtr<FAccessibilityNodeFactory> InSharedPtr)
	{
		ThisPtr = InSharedPtr;
	}

private:

	TSharedPtr<FAccessibilityNodeFactory> ThisPtr;
};
