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

	/**
	 * Creates a Visual Node Widget from the Provided Node Object.
	 * @param InNode The Node To Create a Node Widget From.
	 * @return 
	 */
	virtual TSharedPtr<class SGraphNode> CreateNodeWidget(UEdGraphNode* InNode) override;

	/**
	 * Creates a Visual Pin Widget from the Provided Pin Object.
	 * @param InPin The Pin to Create a Pin Widget From.
	 * @return 
	 */
	virtual TSharedPtr<class SGraphPin> CreatePinWidget(UEdGraphPin* InPin) override;

	/* End Of FGraphNodeFactory Implementation*/

protected:

	/**
	 * The Asset Registry of the Open Accessibility Plugin.
	 */
	TSharedRef<FAssetAccessibilityRegistry> AccessibilityRegistry;
};
