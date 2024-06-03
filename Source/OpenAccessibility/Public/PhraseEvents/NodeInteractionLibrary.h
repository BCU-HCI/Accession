// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PhraseTreeFunctionLibrary.h"

#include "NodeInteractionLibrary.generated.h"

UCLASS()
class UNodeInteractionLibrary : public UPhraseTreeFunctionLibrary
{
	GENERATED_BODY()

public:


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

	UFUNCTION()
	static TSharedPtr<IMenu> NodeAddMenu(FParseRecord& Record);

	UFUNCTION()
    static TSharedPtr<IMenu> NodeAddPinMenu(FParseRecord& Record);

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

};