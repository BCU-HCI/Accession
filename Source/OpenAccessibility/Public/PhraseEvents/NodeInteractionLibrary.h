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
	static void MoveNode(FParseRecord& Record);

	UFUNCTION()
	static void DeleteNode(FParseRecord& Record);

	UFUNCTION()
	static void NodeIndexFocus(int32 Index);

	// End of Node Implementation


	// Pin Implementation

	UFUNCTION()
	static void PinConnect(FParseRecord& Record);

	UFUNCTION()
	static void PinDisconnect(FParseRecord& Record);

	// End of Pin Implementation


	// Node Add Implementation

	static TSharedPtr<IMenu> NodeAddMenu(FParseRecord& Record);

    static TSharedPtr<IMenu> NodeAddPinMenu(FParseRecord& Record);

	
	static void NodeAddSelect(FParseRecord& Record);

	static void NodeAddSearchNew(FParseRecord& Record);

	static void NodeAddSearchAdd(FParseRecord& Record);

	static void NodeAddSearchReset(FParseRecord& Record);

	static void NodeAddScroll(FParseRecord& Record);

	static void NodeAddToggleContext(FParseRecord& Record);

	// End of Node Add Implementation


	// Selection Implementation

	UFUNCTION()
	static void SelectionAdd(FParseRecord& Record);

	UFUNCTION()
	static void SelectionRemove(FParseRecord &Record);

	UFUNCTION()
	static void SelectionReset(FParseRecord &Record);

	UFUNCTION()
	static void SelectionMove(FParseRecord &Record);

	UFUNCTION()
	static void SelectionAlignment(FParseRecord &Record);

	UFUNCTION()
	static void SelectionStraighten(FParseRecord &Record);

	UFUNCTION()
	static void SelectionComment(FParseRecord &Record);

	// End of Selection Implementation


	// Blueprint Specifics

	UFUNCTION()
	static void BlueprintCompile(FParseRecord& Record);

	// End of Blueprint Specifics
};