// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// #include "Containers/Map.h"

/// <summary>
/// A Class that provides a simple way to index objects by a provided key.
/// </summary>
/// <typeparam name="TKey">The Type used for Indexing Assets</typeparam>
/// <typeparam name="TValue">The Type of the Indexed Asset</typeparam>
template < typename TKey, typename TValue >
class OPENACCESSIBILITY_API FAssetIndexer : protected TMap< int, FString >
{

public:
	FAssetIndexer()
	{
		
	}

	~FAssetIndexer()
	{
		this.Contains();
	}

	bool Contains(const TKey& InKey) const
	{
		return true;
	}

	bool FindLinkedKey(const TValue& InValue) const
	{
		return true;
	}

	bool AddAsset(const TKey& Key, const TValue& Value)
	{
		return true;
	}
};
