// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

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

template <class T>
[[nodiscard]] FORCEINLINE TArray<TSharedPtr<T>> GetWidgetDescendants(const TSharedRef<SWidget>& SearchRoot, FString TargetWidgetType)
{
	static_assert(TIsDerivedFrom<T, SWidget>::IsDerived, "Provided Type Is Not a Valid Widget Type.");

	TargetWidgetType.RemoveSpacesInline();

	TArray<TSharedPtr<T>> FoundDescendants = TArray<TSharedPtr<T>>();

	if (SearchRoot->GetTypeAsString() == TargetWidgetType)
		FoundDescendants.Add(StaticCastSharedRef<T>(SearchRoot));

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
					FoundDescendants.Add(StaticCastSharedPtr<T>(CurrentChild));

				ChildrenToSearch.Add(CurrentChild->GetChildren());
			}
		}
	}

	return FoundDescendants;
}

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