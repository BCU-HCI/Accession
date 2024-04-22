// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Map.h"

template<typename IndexValue = UObject*>
class FIndexer
{
public:

	FIndexer();
	~FIndexer();

	bool ContainsKey(const int32& InKey);
	bool ContainsElement(const IndexValue& InElement);

	void GetKey(const IndexValue* InElement, int32& OutKey);
	int32& GetKey(const IndexValue* InElement);

	void GetElement(const int32& InKey, IndexValue& OutElement);
	IndexValue& GetElement(const int32& InKey);

	void AddElement(IndexValue* Widget);
	void RemoveElement(IndexValue* Widget);

	void Reset();
	void Clear();

private:

	int32 GetAvailableKey();
	void GetAvailableKey(int32& OutKey);

protected:
	
	TMap<int32, IndexValue> IndexMap;
};