// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Map.h"

template <typename TKey, typename TValue> class FAssetIndexer
{
	FAssetIndexer() = default;
	virtual ~FAssetIndexer() = default;

	bool AddAssetAtIndex(const TKey& Index, const TValue& AssetToAdd)
	{
		if ( !IndexMap.Contains(Index) )
		{
			IndexMap.Add(Index, TArray<TValue>{AssetToAdd});
			return true;
		}
		else return false;
	}

	bool RemoveKey(const TKey& KeyToRemove)
	{
		if ( IndexMap.Contains(KeyToRemove) )
		{
			IndexMap.Remove(KeyToRemove);
		})

		return true;
	}

	bool IsIndexFree(const TKey& Index) const
	{
		return !IndexMap.Contains(Index);
	}

protected:
	TMap<TKey, TValue> IndexMap;
};
