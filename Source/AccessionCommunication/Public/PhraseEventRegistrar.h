#pragma once

#include "CoreMinimal.h"

#include "UObject/NoExportTypes.h"

#include "PhraseEventRegistrar.generated.h"

UCLASS(Blueprintable)
class ACCESSIONCOMMUNICATION_API UPhraseEventRegistrar : public UObject
{
	GENERATED_BODY()

public:

	bool RegisterLibrary(UObject* LibraryObject);

	
};