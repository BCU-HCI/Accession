// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraph;
class UEdGraphNode;
struct FEdGraphEditAction;

/**
 * 
 */
class OPENACCESSIBILITY_API FGraphIndexer 
{
public:
	FGraphIndexer(const UEdGraph* GraphToIndex);
	~FGraphIndexer();

	/// <summary>
	/// Checks if the Provided Key is Contained within the Indexer.
	/// </summary>
	/// <param name="InKey">The Key To Check if used in the Indexer.</param>
	/// <returns>True, if the Key is Used for Indexing. False, if the Key is Not Used for Indexing.</returns>
	bool ContainsKey(const int& InKey);

	/// <summary>
	/// Checks that the provided Node is Indexed, and retrieves its Key.
	/// </summary>
	/// <param name="InNode">The Node to Find.</param>
	/// <returns>The Key Used to Index The Provided Node. -1 if Unsuccessful in finding the Node.</returns>
	int ContainsNode(UEdGraphNode* InNode);

	/// <summary>
	/// Checks that the provided Node is Indexed, and retrieves its Key.
	/// </summary>
	/// <param name="InNode">The Node to Find.</param>
	/// <param name="OutIndex">The Index Linked to the Provided Node.</param>
	void ContainsNode(UEdGraphNode* InNode, int& OutIndex);

	/// <summary>
	/// Gets the Key Linked to the Provided Node in the Indexer.
	/// </summary>
	/// <param name="InNode">The Node to find the Index of.</param>
	/// <returns>The Index of the Provided Node. -1 on Failure.</returns>
	int GetKey(const UEdGraphNode* InNode);

	/// <summary>
	/// Gets the Key Linked to the Provided Node in the Indexer.
	/// </summary>
	/// <param name="InNode">The Node to find the Index of.</param>
	/// <param name="OutKey">The Index of the Provided Node.</param>
	/// <returns>True, if the Key Was Found. False, if the Key Could Not Be Found.</returns>
	bool GetKey(const UEdGraphNode* InNode, int& OutKey);

	/// <summary>
	/// Gets the Node Linked to the Provided Index.
	/// </summary>
	/// <param name="InIndex">The Index to Find the Node of.</param>
	/// <param name="OutNode">Applies the Found Node, else nullptr.</param>
	void GetNode(const int& InIndex, UEdGraphNode* OutNode);

	/// <summary>
	/// Gets the Node Linked to the Provided Index.
	/// </summary>
	/// <param name="InIndex">The Index to Find the Node of.</param>
	/// <returns>The Found Graph Node, nullptr on Failure.</returns>
	UEdGraphNode* GetNode(const int& InIndex);

	/// <summary>
	/// Gets the Pin Linked to the Provided Index, of the Provided Node Index.
	/// </summary>
	/// <param name="InNodeIndex">The Index of the Node to find the Pin From.</param>
	/// <param name="InPinIndex">The Index of the Pin on the Provided Node.</param>
	/// <param name="OutPin">The Found Pin on the Provided Node.</param>
	void GetPin(const int& InNodeIndex, const int& InPinIndex, UEdGraphPin* OutPin);

	/// <summary>
	/// Gets the Pin Linked to the Provided Index, of the Provided Node Index.
	/// </summary>
	/// <param name="InNodeIndex">The Index of the Node to find the Pin From.</param>
	/// <param name="InPinIndex">The Index of the Pin on the Provided Node.</param>
	/// <returns>The Found Pin on the Provided Node, nullptr on Failure.</returns>
	UEdGraphPin* GetPin(const int& InNodeIndex, const int& InPinIndex);

	/// <summary>
	/// Adds the Given Node to the Indexer.
	/// </summary>
	/// <param name="Node">The Node To Add To The Indexer.</param>
	/// <returns>The Index of the Node in the Indexer.</returns>
	int AddNode(const UEdGraphNode* Node);

	/// <summary>
	/// Adds the Given Node to the Indexer.
	/// </summary>
	/// <param name="OutIndex">The Index of the Node in the Indexer.</param>
	/// <param name="InNode">The Node To Add to the Indexer.</param>
	void AddNode(int& OutIndex, const UEdGraphNode& InNode);

	/// <summary>
	/// Gets or Adds the Provided Node to the Indexer.
	/// </summary>
	/// <param name="InNode">The Node to Get or Look-Up in the Indexer.</param>
	/// <returns>The Index of the Node in the Graph.</returns>
	int GetOrAddNode(const UEdGraphNode* InNode);

	/// <summary>
	/// Gets or Adds the Provided Node to the Indexer.
	/// </summary>
	/// <param name="InNode">The Node to Get or Look-Up in the Indexer.</param>
	/// <param name="OutIndex">The Index of the Node in the Graph.</param>
	void GetOrAddNode(const UEdGraphNode* InNode, int& OutIndex);

	/// <summary>
	/// Removes the Node from the Indexer, linked to the Provided Index.
	/// </summary>
	/// <param name="InIndex">The Index to Remove from the Indexer, and its Linked Node.</param>
	void RemoveNode(const int& InIndex);

	/// <summary>
	/// Removes the Node from the Indexer, finds the Index in the Process.
	/// </summary>
	/// <param name="InNode">The Node To Remove from the Indexer, and its Linked Index.</param>
	void RemoveNode(const UEdGraphNode* InNode);

	/// <summary>
	/// Callback for when the Linked Graph for the Indexer has been Modified.
	/// </summary>
	/// <param name="InAction"></param>
	void OnGraphEvent(const FEdGraphEditAction& InAction);

	/// <summary>
	/// Calls a Full Rebuild of the Indexer, to ensure all Nodes are Indexed.
	/// </summary>
	void OnGraphRebuild();

private:

	/// <summary>
	/// Gets the Next Available Index for the Indexer.
	/// </summary>
	/// <returns>Returns the Next Available Index for the Indexer.</returns>
	int GetAvailableIndex();

	/// <summary>
	/// Gets the Next Available Index for the Indexer.
	/// </summary>
	/// <param name="OutIndex">Applies the Next Available Index.</param>
	void GetAvailableIndex(int& OutIndex);

	/// <summary>
	/// Builds the Graph Index for the Linked Graph.
	/// </summary>
	void BuildGraphIndex();

protected:

	/// <summary>
	/// Map of the Index to the Node.
	/// </summary>
	TMap<int, UEdGraphNode*> IndexMap;

	/// <summary>
	/// Look-Up Set of the Nodes Contained in the Indexer.
	/// </summary>
	TSet<int32> NodeSet;

	/// <summary>
	/// A Queue of the Available Indicies for the Indexer, that was previously in use but made vacant.
	/// </summary>
	TQueue<int32> AvailableIndices;

	/// <summary>
	/// The Graph Being Indexed By This Indexer.
	/// </summary>
	UEdGraph* LinkedGraph;

	FDelegateHandle OnGraphChangedHandle;
};
