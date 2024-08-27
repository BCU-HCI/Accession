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

	/**
	 * Binds Branches originating from this Library onto the provided Phrase Tree.
	 * @param PhraseTree Reference to the PhraseTree to Bind this Library to.
	 */
	virtual void BindBranches(TSharedRef<FPhraseTree> PhraseTree) override;

	// End of UPhraseTreeFunctionLibrary Implementation


	// Node Implementation

	/**
	 * Phrase Event for Moving a Node, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void MoveNode(FParseRecord& Record);

	/**
	 * Phrase Event for Deleting a Node, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void DeleteNode(FParseRecord& Record);

	/**
	 * Input Event for Adding the specified Node Index to the Active Selection Set.
	 * @param Index The Index Provided Through Voice Input. 
	 */
	UFUNCTION()
	void NodeIndexFocus(int32 Index);

	// End of Node Implementation


	// Pin Implementation

	/**
	 * Phrase Event for Connecting Two Provided Pins, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void PinConnect(FParseRecord& Record);

	/**
	 * Phrase Event for Disconnecting Two Provided Pins, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void PinDisconnect(FParseRecord& Record);

	UFUNCTION()
	void PinSetDefault(FParseRecord& Record);

	UFUNCTION()
	void PinResetDefault(FParseRecord& Record);

	// End of Pin Implementation


	// Node Add Implementation

	/**
	 * Menu Event for Initializing the Node Add Context Menu, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 * @return A Shared Pointer to the Initialized Menu, otherwise an Invalid Shared Pointer.
	 */
	TSharedPtr<IMenu> NodeAddMenu(FParseRecord& Record);

	/**
     * Menu Event for Initializing the Node Add Context Menu from a Pin Connection, on the Active Graph Editor.
     * @param Record The Parse Record accumulated until this Event.
     * @return A Shared Pointer to the Initialized Menu, otherwise an Invalid Shared Pointer.
     */
    TSharedPtr<IMenu> NodeAddPinMenu(FParseRecord& Record);

	
	/**
	 * Phrase Event for Selecting an Item on the Active Graph Editors Node Add Context Menu.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void NodeAddSelect(FParseRecord& Record);

	/**
	 * Phrase Event for Appending Strings to the SearchBar on the Active Graph Editors Node Add Context Menu.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void NodeAddSearchAdd(FParseRecord& Record);

	/**
	 * Phrase Event for Removing String Chunks on the SearchBar of the Active Graph Editors Node Add Context Menu.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void NodeAddSearchRemove(FParseRecord& Record);

	/**
	 * Phrase Event for Resetting the SearchBar of the Active Graph Editors Node Add Context Menu.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void NodeAddSearchReset(FParseRecord& Record);

	/**
	 * Phrase Event for Applying Movement to the Scrollbar of the Active Graph Editors Node Add Context Menu.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	void NodeAddScroll(FParseRecord& Record);

	// End of Node Add Implementation


	// Selection Implementation

	/**
	 * Phrase Event for Toggling the specified Nodes Selection State, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void SelectionNodeToggle(FParseRecord& Record);

	/**
	 * Phrase Event for Resetting the Selection Set, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void SelectionReset(FParseRecord &Record);

	/**
	 * Phrase Event for Moving the Selection Set, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void SelectionMove(FParseRecord &Record);

	/**
	 * Phrase Event for Aligning the Selection Sets Nodes, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void SelectionAlignment(FParseRecord &Record);

	/**
	 * Phrase Event for Straightening the Selection Sets Connections, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void SelectionStraighten(FParseRecord &Record);

	/**
	 * Phrase Event for Applying a Comment Node Around the Selection Set, on the Active Graph Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void SelectionComment(FParseRecord &Record);

	// End of Selection Implementation


	// Locomotion Implementation

	/**
	 * Phrase Event for Selecting a Viewport Rect for Movement, on the Active Graph Editors Locomotion Mode.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void LocomotionSelect(FParseRecord& Record);

	/**
	 * Phrase Event for Reverting the Viewport to the Previous Rect, on the Active Graph Editors Locomotion Mode.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void LocomotionRevert(FParseRecord& Record);

	/**
	 * Phrase Event for Confirming the Current Viewport, on the Active Graph Editors Locomotion Mode.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void LocomotionConfirm(FParseRecord& Record);

	/**
	 * Phrase Event for Canceling the Active Graph Editors Locomotion Mode, reverting to viewport state before.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void LocomotionCancel(FParseRecord& Record);

	// End of Locomotion Implementations


	// Blueprint Specifics

	/**
	 * Phrase Event for Compiling Blueprint Linked to the Active Blueprint Editor.
	 * @param Record The Parse Record accumulated until this Event.
	 */
	UFUNCTION()
	void BlueprintCompile(FParseRecord& Record);

	// End of Blueprint Specifics
};