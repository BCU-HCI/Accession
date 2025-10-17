// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

#include "PhraseTree.h"

#include "PhraseTree/Containers/ParseRecord.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"
#include "PhraseTree/Containers/Input/UParseStringInput.h"
#include "PhraseTree/Containers/Input/UParseEnumInput.h"

#include "PhraseTreeFunctionLibrary.generated.h"

// Utilities

ACCESSIONCOMMUNICATION_API DECLARE_LOG_CATEGORY_EXTERN(LogAccessionPhraseEvent, Log, All);

// Phrase Tree Library Base Functions

UCLASS(Abstract)
class ACCESSIONCOMMUNICATION_API UPhraseTreeFunctionLibrary : public UObject
{
    GENERATED_BODY()

public:
    virtual void BindBranches(TSharedRef<FPhraseTree> PhraseTree) {};
};