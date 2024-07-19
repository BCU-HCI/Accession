// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/Containers/ContextMenuObject.h"

#include "SGraphActionMenu.h"
#include "GraphActionNode.h"

#include "AccessibilityGraphEditorContext.generated.h"

UCLASS()
class OPENACCESSIBILITY_API UAccessibilityGraphEditorContext : public UPhraseTreeContextMenuObject
{
	GENERATED_BODY()

public:

	UAccessibilityGraphEditorContext();
	UAccessibilityGraphEditorContext(TSharedRef<IMenu> Menu);


	// -- UPhraseTreeContextMenuObject Implementation

	virtual void Init(TSharedRef<IMenu> InMenu, TSharedRef<FPhraseNode> InContextRoot) override;

	virtual bool Tick(float DeltaTime) override;

	virtual bool Close() override;

	virtual void ScaleMenu(const float ScaleFactor = 1.5f) override;

	// -- End of UPhraseTreeContextMenuObject Implementation

protected:

	bool FindGraphActionMenu(const TSharedRef<SWidget>& SearchRoot);

	bool FindTreeView(const TSharedRef<SWidget>& SearchRoot);

	bool FindCheckBoxes(const TSharedRef<SWidget>& SearchRoot);

protected:

	TWeakPtr<SGraphActionMenu> GraphMenu = TWeakPtr<SGraphActionMenu>();
	TWeakPtr<SEditableTextBox> FilterTextBox = TWeakPtr<SEditableTextBox>();

	TWeakPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeView = TWeakPtr<STreeView<TSharedPtr<FGraphActionNode>>>();

	TArray<TWeakPtr<SCheckBox>> CheckBoxes = TArray<TWeakPtr<SCheckBox>>();
};