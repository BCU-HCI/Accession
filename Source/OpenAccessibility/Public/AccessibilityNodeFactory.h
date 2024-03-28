// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NodeFactory.h"

class FAssetAccessibilityRegistry;

/**
 * 
 */
class OPENACCESSIBILITY_API FGraphAccessibilityNodeFactory : public FGraphNodeFactory
{
public:

	FGraphAccessibilityNodeFactory();
	FGraphAccessibilityNodeFactory(TSharedRef<FAssetAccessibilityRegistry> InAccessibilityRegistry);
	virtual ~FGraphAccessibilityNodeFactory();

	/* FGraphNodeFactory Implementation */

	virtual TSharedPtr<class SGraphNode> CreateNodeWidget(UEdGraphNode* InNode) override;

	virtual TSharedPtr<class SGraphPin> CreatePinWidget(UEdGraphPin* InPin) override;

	/* End Of FGraphNodeFactory Implementation*/

protected:
	TSharedRef<FAssetAccessibilityRegistry> AccessibilityRegistry;
};
