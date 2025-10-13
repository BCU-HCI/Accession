// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"

#include "AccessionLogging.h"

/**
 * A Templated Indexer for Indexing Assets in a TMap.
 * @tparam KeyType Type of the Key Element of the Index.
 * @tparam ValueType Type of the Value Element of the Index.
 */
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

	/**
	 * Checks if the Indexer is Empty.
	 * @return True if the Indexer is Empty, otherwise False.
	 */
	bool IsEmpty() const
	{
		return IndexMap.IsEmpty();
	}

	/**
	 * Empties the Indexer, but preserves all Allocations.
	 */
	void Reset()
	{
		IndexMap.Reset();
		AvailableIndexes.Empty();
	}

	/**
	 * Empties the Indexer, preserving no space allocated.
	 */
	void Empty()
	{
		IndexMap.Empty();
		AvailableIndexes.Empty();
	}

	/**
	 * Gets the Number of Items Currently in the Indexer.
	 * @return Number of Items being Indexed.
	 */
	int32 Num() const
	{
		return IndexMap.Num();
	}

	/**
	 * Gets the Number of Items Currently in the Indexer.
	 * @param OutNum Sets to the Number of Items Being Indexed.
	 */
	void Num(int32 &OutNum) const
	{
		OutNum = IndexMap.Num();
	}

	/**
	 * Checks if the indexer contains the specified key.
	 * @param InKey The Key to Search For.
	 * @return True if the Key is in use in the Indexer, otherwise False.
	 */
	bool ContainsKey(const KeyType &InKey)
	{
		return IndexMap.Contains(InKey);
	}

	/**
	 * Checks if the Indexer contains the specified value.
	 * @param InValue The Value to Search For.
	 * @return True of the  specified value is associated with the Indexer.
	 */
	bool ContainsValue(const ValueType &InValue)
	{
		check(InValue != nullptr);

		const KeyType *FoundKey = IndexMap.FindKey(InValue);

		return FoundKey != nullptr;
	}

	/**
	 * Gets the associated Key with the specified value.
	 * @param InValue The value to search using.
	 * @return The associated key for the specified value.
	 */
	const KeyType GetKey(const ValueType &InValue)
	{
		check(InValue != nullptr);

		return *IndexMap.FindKey(InValue);
	}

	/**
	 * Gets the associated Key with the specified value.
	 * @param InValue The value to search using.
	 * @param OutKey Sets the associated key for the specified value
	 * @return True if the associated key was found, otherwise False.
	 */
	bool GetKey(const ValueType &InValue, KeyType &OutKey)
	{
		check(InValue != nullptr);

		const KeyType *FoundKey = IndexMap.FindKey(InValue);

		if (FoundKey != nullptr)
		{
			OutKey = *FoundKey;

			return true;
		}
		else
			return false;
	}

	/**
	 * Gets the value linked to the specified key.
	 * @param InKey The Key to Search using.
	 * @return The associated value of the specified key.
	 */
	ValueType GetValue(const KeyType &InKey)
	{
		return *IndexMap.Find(InKey);
	}

	/**
	 * Gets the value linked to the specified key.
	 * @param InKey The Key to Search using.
	 * @param OutValue Sets the associated value of the specified key.
	 * @return True if an associated value was found, otherwise False.
	 */
	bool GetValue(const KeyType &InKey, ValueType &OutValue)
	{
		if (!IndexMap.Contains(InKey))
		{
			UE_LOG(LogAccession, Warning, TEXT("Provided Key is not recognised."));
			return false;
		}

		OutValue = *IndexMap.Find(InKey);

		return true;
	}

	/**
	 * Inserts the specified value into the indexer, and provides its key.
	 * @param InValue The value to insert.
	 * @return The Key of the associated to the inserted value in the indexer.
	 */
	KeyType AddValue(const ValueType &InValue)
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

	/**
	 * Inserts the specified value into the indexer, and provides its key.
	 * @param InValue The value to insert.
	 * @param OutKey The Key of the associated to the newly inserted value.
	 */
	void AddValue(const ValueType &InValue, KeyType &OutKey)
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

	/**
	 * Finds or Inserts the specified value into the Indexer.
	 * @param InValue The value to find or insert into the indexer.
	 * @return The Key of the associated value.
	 */
	KeyType GetKeyOrAddValue(const ValueType &InValue)
	{
		check(InValue != nullptr);

		KeyType FoundKey;
		if (GetKey(InValue, FoundKey))
			return FoundKey;

		return AddValue(InValue);
	}

	/**
	 * Finds or Inserts the specified value into the Indexer.
	 * @param InValue The value to find or insert into the indexer.
	 * @param OutKey Sets the Key of the associated value.
	 */
	void GetKeyOrAddValue(const ValueType &InValue, KeyType &OutKey)
	{
		check(InValue != nullptr);

		if (GetKey(InValue, OutKey))
			return;

		OutKey = AddValue(InValue);
	}

	/**
	 * Removes the specified key from the Indexer.
	 * @param InKey The key to remove from the indexer.
	 */
	void RemoveValue(const KeyType &InKey)
	{
		if (!IndexMap.Contains(InKey))
		{
			UE_LOG(LogAccession, Warning, TEXT("Provided Key Has No Pair in Index."));
			return;
		}

		IndexMap.Remove(InKey);
		AvailableIndexes.Enqueue(InKey);
	}

	/**
	 * Removes the specified value and its associated key from the Indexer.
	 * @param InValue The value to remove from the Indexer.
	 */
	void RemoveValue(const ValueType &InValue)
	{
		check(InValue != nullptr);

		KeyType FoundKey;
		if (GetKey(InValue, FoundKey))
		{
			IndexMap.Remove(FoundKey);
			AvailableIndexes.Enqueue(FoundKey);
		}
		else
			UE_LOG(LogAccession, Log, TEXT("Provided Value Had No Associated Key."));
	}

protected:
	/**
	 * Gets the Next Available Key in the Indexer.
	 * @param OutKey Sets the Next Available Key.
	 */
	void GetAvailableKey(KeyType &OutKey)
	{
		if (!AvailableIndexes.IsEmpty() && AvailableIndexes.Dequeue(OutKey))
			return;

		OutKey = IndexMap.Num();
	}

	/**
	 * Gets the Next Available Key in the Indexer.
	 * @return The next available key in the indexer.
	 */
	KeyType GetAvailableKey()
	{
		if (!AvailableIndexes.IsEmpty())
		{
			KeyType OutKey;
			if (AvailableIndexes.Dequeue(OutKey))
				return OutKey;
		}

		return IndexMap.Num();
	}

public:
protected:
	/**
	 * The Map of Keys to Associated Values.
	 */
	TMap<KeyType, ValueType> IndexMap;

	/**
	 * The Queue of Available Indexes from Previous Associations.
	 */
	TQueue<KeyType> AvailableIndexes;
};