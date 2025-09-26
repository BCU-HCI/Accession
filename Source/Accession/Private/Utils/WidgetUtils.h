// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#pragma once

#include "CoreMinimal.h"

/**
 * Finds a Widget of the Target Type descending from the Search Root.
 * @tparam T Target Widgets Type (Must be a Widget Type)
 * @param SearchRoot The Widget to Perform the Search From.
 * @param TargetWidgetType The Target Widgets Type, in String Format.
 * @return A SharedPtr of a Found Target Descendant, on failure an invalid pointer is returned.
 */
template<class T>
[[nodiscard]] FORCEINLINE TSharedPtr<T> GetWidgetDescendant(const TSharedRef<SWidget>& SearchRoot, FString TargetWidgetType)
{
	static_assert(TIsDerivedFrom<T, SWidget>::IsDerived, "Provided Type Is Not a Valid Widget Type.");

	TargetWidgetType.RemoveSpacesInline();

	if (SearchRoot->GetType() == TargetWidgetType)
		return StaticCastSharedRef<T>(SearchRoot);

	{
		TArray<FChildren*> ChildrenToSearch = TArray{
			SearchRoot->GetChildren()
		};

		FChildren* CurrentChildren;
		TSharedPtr<SWidget> CurrentChild;
		FString CurrentChildString;

		while (ChildrenToSearch.Num() > 0)
		{
			CurrentChildren = ChildrenToSearch.Pop();

			for (int i = 0; i < CurrentChildren->Num(); i++)
			{
				CurrentChild = CurrentChildren->GetChildAt(i);

				CurrentChildString = CurrentChild->GetTypeAsString();
				CurrentChildString.RemoveSpacesInline();

				if (CurrentChildString == TargetWidgetType)
					return StaticCastSharedPtr<T>(CurrentChild);

				ChildrenToSearch.Add(CurrentChild->GetChildren());
			}
		}
	}

	return TSharedPtr<T>();
}


/**
 * Finds all widgets of the Target Type descending from the Search Root.
 * @tparam T Target Widgets Type (Must be a Widget Type)
 * @param SearchRoot The Widget To Perform the Search From.
 * @param TargetWidgetType The Target Widgets Type, in String Format.
 * @return An Array of Shared Pointers of the Target Widget Type.
 */
template <class T>
[[nodiscard]] FORCEINLINE TArray<TSharedPtr<T>> GetWidgetDescendants(const TSharedRef<SWidget>& SearchRoot, FString TargetWidgetType)
{
	static_assert(TIsDerivedFrom<T, SWidget>::IsDerived, "Provided Type Is Not a Valid Widget Type.");

	TargetWidgetType.RemoveSpacesInline();

	TArray<TSharedPtr<T>> FoundDescendants = TArray<TSharedPtr<T>>();

	if (SearchRoot->GetTypeAsString() == TargetWidgetType)
		FoundDescendants.Add(StaticCastSharedRef<T>(SearchRoot));

	{
		TArray<FChildren*> ChildrenToSearch = TArray {
			SearchRoot->GetChildren()
		};

		while (ChildrenToSearch.Num() > 0)
		{
			FChildren* CurrentChildren = ChildrenToSearch.Pop();

			for (int i = 0; i < CurrentChildren->Num(); i++)
			{
				TSharedPtr<SWidget> CurrentChild = CurrentChildren->GetChildAt(i);

				FString CurrentChildString = CurrentChild->GetTypeAsString();
				CurrentChildString.RemoveSpacesInline();

				if (CurrentChildString == TargetWidgetType)
					FoundDescendants.Add(StaticCastSharedPtr<T>(CurrentChild));

				ChildrenToSearch.Add(CurrentChild->GetChildren());
			}
		}
	}

	return FoundDescendants;
}

/**
 * Finds All Widget Slots Containing a Widget of a Target Type. Descending from the Search Root.
 * @param SearchRoot The Starting Point For The Widget Search
 * @param TargetTypes A Set Containing The Target Widget Types To Find.
 * @return An Array of Widget Slots, Containing Widgets of a Target Type.
 */
[[nodiscard]] FORCEINLINE TArray<FSlotBase*> GetWidgetSlotsByType(const TSharedRef<SWidget>& SearchRoot, const TSet<FString>& TargetTypes)
{
	TArray<FSlotBase*> FoundDescendants = TArray<FSlotBase*>();

	{
		TArray<FChildren*> ChildrenToSearch = TArray{
			SearchRoot->GetChildren()
		};

		FChildren* CurrentChildren;
		FString CurrentWidgetString;

		while (ChildrenToSearch.Num() > 0)
		{
			CurrentChildren = ChildrenToSearch.Pop();

			for (int i = 0; i < CurrentChildren->NumSlot(); i++)
			{
				FSlotBase& CurrentSlot = const_cast<FSlotBase&>(CurrentChildren->GetSlotAt(i));

				const TSharedRef<SWidget> CurrentWidget = CurrentSlot.GetWidget();

				CurrentWidgetString = CurrentWidget->GetTypeAsString();
				CurrentWidgetString.RemoveSpacesInline();

				if (TargetTypes.Contains(CurrentWidgetString))
					FoundDescendants.Add(&CurrentSlot);

				ChildrenToSearch.Add(CurrentWidget->GetChildren());
			}
		}
	}

	return FoundDescendants;
}