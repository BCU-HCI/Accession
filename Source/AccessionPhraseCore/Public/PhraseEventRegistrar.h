#pragma once

#include "CoreMinimal.h"

#include "UObject/NoExportTypes.h"

#include "PhraseEventRegistrar.generated.h"

class UPhraseTreeFunctionLibrary;
struct FPhraseEvent;

UCLASS(Blueprintable)
class ACCESSIONPHRASECORE_API UPhraseEventRegistrar : public UObject
{
	GENERATED_BODY()

public:

	bool RegisterLibrary(UObject* LibraryObject);
	bool RegisterLibrary(UPhraseTreeFunctionLibrary* LibraryObject);

private:

	
	bool DiscoverPhraseEvents(UPhraseTreeFunctionLibrary* LibraryObject, UClass* LibraryClass);
	bool TryRegisterFunction(const UPhraseTreeFunctionLibrary* Library, const UFunction* Function);

	FName GetMetaName(const UFunction* Function, const TCHAR* Key) const;
	bool GetMetaBool(const UFunction* Function, const TCHAR* Key) const;

private:

	UPROPERTY()
	TArray<FPhraseEvent> DiscoveredEvents;

	UPROPERTY()
	TSet<FName> RegisteredEventNames;

};