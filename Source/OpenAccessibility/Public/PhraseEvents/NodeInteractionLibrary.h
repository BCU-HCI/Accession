// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTree/PhraseTreeFunctionLibrary.h"

#include "NodeInteractionLibrary.generated.h"

UCLASS()
class UNodeInteractionLibrary : public UPhraseTreeFunctionLibrary
{
	GENERATED_BODY()

public:

	UNodeInteractionLibrary(const FObjectInitializer& ObjectInitializer);

	virtual ~UNodeInteractionLibrary();

	// UPhraseTreeFunctionLibrary Implementation

	void BindBranches(TSharedRef<FPhraseTree> PhraseTree) override;

	// End of UPhraseTreeFunctionLibrary Implementation


	// Node Implementation

	UFUNCTION()
	void MoveNode(FParseRecord& Record);

	UFUNCTION()
	void DeleteNode(FParseRecord& Record);

	UFUNCTION()
	void NodeIndexFocus(int32 Index);

	// End of Node Implementation


	// Pin Implementation

	UFUNCTION()
	void PinConnect(FParseRecord& Record);

	UFUNCTION()
	void PinDisconnect(FParseRecord& Record);

	// End of Pin Implementation


	// Node Add Implementation

	TSharedPtr<IMenu> NodeAddMenu(FParseRecord& Record);

    TSharedPtr<IMenu> NodeAddPinMenu(FParseRecord& Record);

	
	void NodeAddSelect(FParseRecord& Record);

	void NodeAddSearchNew(FParseRecord& Record);

	void NodeAddSearchAdd(FParseRecord& Record);

	void NodeAddSearchReset(FParseRecord& Record);

	void NodeAddScroll(FParseRecord& Record);

	void NodeAddToggleContext(FParseRecord& Record);

	// End of Node Add Implementation


	// Selection Implementation

	UFUNCTION()
	void SelectionAdd(FParseRecord& Record);

	UFUNCTION()
	void SelectionRemove(FParseRecord &Record);

	UFUNCTION()
	void SelectionReset(FParseRecord &Record);

	UFUNCTION()
	void SelectionMove(FParseRecord &Record);

	UFUNCTION()
	void SelectionAlignment(FParseRecord &Record);

	UFUNCTION()
	void SelectionStraighten(FParseRecord &Record);

	UFUNCTION()
	void SelectionComment(FParseRecord &Record);

	// End of Selection Implementation


	// Locomotion Implementation

	UFUNCTION()
	void LocomotionSelect(FParseRecord& Record);

	UFUNCTION()
	void LocomotionRevert(FParseRecord& Record);

	UFUNCTION()
	void LocomotionConfirm(FParseRecord& Record);

	UFUNCTION()
	void LocomotionClose(FParseRecord& Record);

	// End of Locomotion Implementations


	// Blueprint Specifics

	UFUNCTION()
	void BlueprintCompile(FParseRecord& Record);

	// End of Blueprint Specifics
};