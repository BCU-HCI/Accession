// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

/**
 *
 */
class ACCESSION_API FAccessionNodeFactory : public FGraphPanelNodeFactory
{

public:
	/* Begin FGraphPanelNodeFactory */
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode *Node) const override;
	/* End FGraphPanelNodeFactory */

public:
	FAccessionNodeFactory();
	virtual ~FAccessionNodeFactory();

	/// <summary>
	/// Wraps the Node Widget with Indexing.
	/// </summary>
	/// <param name="Node">The Node Object That is Being Wrapped.</param>
	/// <param name="NodeWidget">The Node Widget That is Being Wrapped.</param>
	/// <param name="NodeIndex">The Index of the Node.</param>
	inline void WrapNodeWidget(UEdGraphNode *Node, TSharedRef<SGraphNode> NodeWidget, int NodeIndex) const;

	/// <summary>
	/// Wraps the Pin Widget with Indexing.
	/// </summary>
	/// <param name="Pin">The Pin Object That is Being Wrapped.</param>
	/// <param name="PinWidget">The Node Widget That is Being Wrapped.</param>
	/// <param name="PinIndex">The Index of the Pin.</param>
	/// <param name="OwnerNode">The Owning Node Widget of the Pin.</param>
	inline void WrapPinWidget(UEdGraphPin *Pin, TSharedRef<SGraphPin> PinWidget, int PinIndex, SGraphNode *OwnerNode) const;

	void SetSharedPtr(TSharedPtr<FAccessionNodeFactory> InSharedPtr)
	{
		ThisPtr = InSharedPtr;
	}

private:
	TSharedPtr<FAccessionNodeFactory> ThisPtr;
};
