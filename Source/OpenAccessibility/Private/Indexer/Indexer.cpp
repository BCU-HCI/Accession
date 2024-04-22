// Copyright F-Dudley. All Rights Reserved.

#include "Indexer/Indexer.h"

/*
template<typename IndexValue>
FIndexer<IndexValue>::FIndexer()
{

}

template<typename IndexValue>
FIndexer<IndexValue>::~FIndexer()
{
	IndexMap.Empty();
}

template<typename IndexValue>
bool FIndexer<IndexValue>::ContainsKey(const int32& InKey)
{
	return IndexMap.Contains(InKey);
}

template<typename IndexValue>
bool FIndexer<IndexValue>::ContainsElement(const IndexValue& InElement)
{
	check(InValue != nullptr);

	const IndexKey* ReturnedIndex = IndexMap.FindAndRemoveChecked(InElement);

	if (ReturnedIndex != nullptr)
		return true;
	else return false;
}

template<typename IndexValue>
void FIndexer<IndexValue>::GetKey(const IndexValue* InElement, int32& OutKey)
{
	check(InValue != nullptr);

	const int32* ReturnedIndex = IndexMap.FindKey(InElement);

	if (ReturnedIndex != nullptr)
		OutKey = *ReturnedIndex;
	else OutKey = -1;
}

template<typename IndexValue>
int32& FIndexer<IndexValue>::GetKey(const IndexValue* InElement)
{
	check(InElement != nullptr);

	const int32* FoundKey = IndexMap.FindKey(const_cast<IndexValue*>(InElement));
	if (FoundKey != nullptr)
		return *FoundKey;
	else return -1;
}
*/
