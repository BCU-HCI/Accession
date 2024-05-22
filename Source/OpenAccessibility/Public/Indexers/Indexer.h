// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "OpenAccessibilityLogging.h"

template <typename KeyType, typename ValueType>
class FIndexer
{
public:

	FIndexer()
	{

	}

	virtual ~FIndexer()
	{

	}

	void Empty()
	{
		IndexMap.Reset();
		AvailableIndexes.Empty();
	}

	bool ContainsKey(const KeyType& InKey)
	{
		return IndexMap.Contains(InKey);
	}

	bool ContainsValue(const ValueType& InValue)
	{
		check(InValue != nullptr);

		const KeyType* FoundKey = IndexMap.FindKey(InValue);

		return FoundKey != nullptr;
	}

	const KeyType GetKey(const ValueType& InValue)
	{
		check(InValue != nullptr);

		return *IndexMap.FindKey(InValue);
	}

	bool GetKey(const ValueType& InValue, KeyType& OutKey)
	{
		check(InValue != nullptr);

		const KeyType* FoundKey = IndexMap.FindKey(InValue);

		if (FoundKey != nullptr)
		{
			OutKey = *FoundKey;

			return true;
		}
		else return false;
	}

	ValueType GetValue(const KeyType& InKey)
	{
		return *IndexMap.Find(InKey);
	}

	bool GetValue(const KeyType& InKey, ValueType& OutValue)
	{
		if (!IndexMap.Contains(InKey))
		{
			UE_LOG(LogOpenAccessibility, Warning, TEXT("Provided Key is not recognised."));
			return false;
		}

		OutValue = *IndexMap.Find(InKey);

		return true;
	}


	KeyType AddValue(const ValueType& InValue)
	{
		check(InValue != nullptr);

		if (ContainsValue(InValue))
		{
			return GetKey(InValue);
		}

		KeyType NewKey;
		GetAvailableKey(NewKey);

		IndexMap.Add(NewKey, InValue);

		return NewKey;
	}

	void AddValue(const ValueType& InValue, KeyType& OutKey)
	{
		check(InValue != nullptr);

		if (ContainsValue(InValue))
		{
			OutKey = GetKey(InValue);
			return;
		}

		OutKey = GetAvailableKey();

		IndexMap.Add(OutKey, InValue);
	}

	KeyType GetKeyOrAddValue(const ValueType& InValue)
	{
		check(InValue != nullptr);

		KeyType FoundKey;
		if (GetKey(InValue, FoundKey))
			return FoundKey;

		return AddValue(InValue);
	}

	void GetKeyOrAddValue(const ValueType& InValue, KeyType& OutKey)
	{
		check(InValue != nullptr);

		if (GetKey(InValue, OutKey))
			return;

		OutKey = AddValue(InValue);
	}

	void RemoveValue(const KeyType& InKey)
	{
		if (!IndexMap.Contains(InKey))
		{
			UE_LOG(LogOpenAccessibility, Warning, TEXT("Provided Key Had Not Pair in Index."));
			return;
		}

		IndexMap.Remove(InKey);
		AvailableIndexes.Enqueue(InKey);
	}
	
	void RemoveValue(const ValueType& InValue)
	{
		check(InValue != nullptr);

		KeyType FoundKey;
		if (GetKey(InValue, FoundKey))
		{
			IndexMap.Remove(FoundKey);
			AvailableIndexes.Enqueue(FoundKey);
		}
		else UE_LOG(LogOpenAccessibility, Log, TEXT("Provided Value Had No Associated Key."));
	}

protected:

	void GetAvailableKey(KeyType& OutKey)
	{
		if (!AvailableIndexes.IsEmpty() && AvailableIndexes.Dequeue(OutKey))
			return;
		else OutKey = IndexMap.Num();
	}

	KeyType GetAvailableKey()
	{
		if (!AvailableIndexes.IsEmpty())
		{
			KeyType OutKey;
			if (AvailableIndexes.Dequeue(OutKey));
				return OutKey;
		}

		return IndexMap.Num();
	}

public:


protected:

	TMap<KeyType, ValueType> IndexMap;

	TQueue<KeyType> AvailableIndexes;
};