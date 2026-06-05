#include "PhraseEventRegistrar.h"

#include "PhraseTree/PhraseTreeFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogPhraseEventRegistrar, Log, All);

USTRUCT()
struct FPhraseEvent
{
	FName CommandId;
	FName ContextId;

	FString DefaultPhrase;
	FString OverridePhrase = FString();

	const UPhraseTreeFunctionLibrary* OwningLibrary;
	const UFunction* Function;
};

bool UPhraseEventRegistrar::RegisterLibrary(UObject* LibraryObject)
{
	if (!LibraryObject)
	{
		UE_LOG(LogPhraseEventRegistrar, Warning, TEXT("Tried to Register NULL Library Object."))
			return  false;
	}

	if (!LibraryObject->IsA<UPhraseTreeFunctionLibrary>())
	{
		UE_LOG(LogPhraseEventRegistrar, Warning, TEXT("Tried to Register Object that is not a Phrase Tree Function Library: %s"), *LibraryObject->GetName())
			return false;
	}

	UClass* LibraryClass = LibraryObject->GetClass();
	return DiscoverPhraseEvents(Cast<UPhraseTreeFunctionLibrary>(LibraryObject), LibraryClass);
}

bool UPhraseEventRegistrar::RegisterLibrary(UPhraseTreeFunctionLibrary* LibraryObject)
{
	if (!LibraryObject)
	{
		UE_LOG(LogPhraseEventRegistrar, Warning, TEXT("Tried to Register NULL Library Object."))
			return  false;
	}

	UClass* LibraryClass = LibraryObject->GetClass();
	DiscoverPhraseEvents(LibraryObject, LibraryClass);

	return DiscoverPhraseEvents(LibraryObject, LibraryClass);
}

bool UPhraseEventRegistrar::DiscoverPhraseEvents(UPhraseTreeFunctionLibrary* LibraryObject, UClass* LibraryClass)
{
	check(LibraryClass);

	bool bFoundAny = false;

	for (TFieldIterator<UFunction> It(LibraryClass, EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		const UFunction* Function = *It;

		if (!Function || !Function->HasMetaData(TEXT("AccessionPhraseEvent")))
			continue;

		bFoundAny |= TryRegisterFunction(LibraryObject, Function);
	}
}

bool UPhraseEventRegistrar::TryRegisterFunction(const UPhraseTreeFunctionLibrary* Library, const UFunction* Function)
{
	const FName CommandId = GetMetaName(Function, TEXT("AccessionCommandId"));
	const FString DefaultPhrase = Function->GetMetaData(TEXT("AccessionDefaultPhrase"));
		
	if (CommandId.IsNone())
	{
		UE_LOG(LogPhraseEventRegistrar, Warning, TEXT("Function %s in Library %s is marked as an Accession Phrase Event but does not have an AccessionCommandId metadata value."), *Function->GetName(), *Library->GetName());
		return false;
	}

	if (DefaultPhrase.IsEmpty())
	{
		UE_LOG(LogPhraseEventRegistrar, Warning, TEXT("Function %s in Library %s is marked as an Accession Phrase Event but does not have an AccessionDefaultPhrase metadata value."), *Function->GetName(), *Library->GetName());
		return false;
	}

	if (RegisteredEventNames.Contains(CommandId))
	{
		UE_LOG(LogPhraseEventRegistrar, Warning, TEXT("Function %s in Library %s is marked as an Accession Phrase Event but has a duplicate AccessionCommandId metadata value of %s."), *Function->GetName(), *Library->GetName(), *CommandId.ToString());
		return false;
	}

	FPhraseEvent NewEvent;
	NewEvent.CommandId = CommandId;
	NewEvent.DefaultPhrase = DefaultPhrase;
	NewEvent.OwningLibrary = Library;
	NewEvent.Function = Function;

	if (Function->HasMetaData(TEXT("AccessionContextId")))
	{
		const FString ContextId = Function->GetMetaData(TEXT("AccessionContextId"));
		NewEvent.ContextId = FName(*ContextId);
	}
	else 
	{
		NewEvent.ContextId = FName("Global");
	}

	// Insert Saved Override Event String etc.


	DiscoveredEvents.Add(NewEvent);
	RegisteredEventNames.Add(CommandId);

	UE_LOG(LogPhraseEventRegistrar, Log, 
		TEXT("Discovered Phrase Event: %s | Function=%s | Context=%s | Phrase=\"%s\""),
		*CommandId.ToString(), 
		*Function->GetName(), 
		*NewEvent.ContextId.ToString(), 
		*DefaultPhrase
	);

	return true;
}

FName UPhraseEventRegistrar::GetMetaName(const UFunction* Function, const TCHAR* Key) const
{
	if (!Function || !Function->HasMetaData(Key))
	{
		return NAME_None;
	}

	const FString Value = Function->GetMetaData(Key);
	if (Value.IsEmpty())
	{
		return NAME_None;
	}

	return FName(*Value);
}

bool UPhraseEventRegistrar::GetMetaBool(const UFunction* Function, const TCHAR* Key) const
{
	if (!Function || !Function->HasMetaData(Key))
	{
		return false;
	}

	const FString Value = Function->GetMetaData(Key);
	if (Value.IsEmpty())
	{
		return false;
	}

	return Value.Equals(TEXT("true"), ESearchCase::IgnoreCase) || Value.Equals(TEXT("1"), ESearchCase::IgnoreCase);
}