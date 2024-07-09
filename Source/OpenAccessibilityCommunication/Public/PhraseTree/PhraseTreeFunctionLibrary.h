// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/UnrealType.h"
#include "UObject/ScriptMacros.h"

#include "PhraseTree.h"

#include "PhraseTree/Containers/ParseRecord.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"
#include "PhraseTree/Containers/Input/UParseStringInput.h"
#include "PhraseTree/Containers/Input/UParseEnumInput.h"

#include "PhraseTreeFunctionLibrary.generated.h"

// Utility Definitions

DECLARE_LOG_CATEGORY_EXTERN(LogOpenAccessibilityPhraseEvent, Log, All);

DEFINE_LOG_CATEGORY(LogOpenAccessibilityPhraseEvent);

UCLASS(Abstract)
class OPENACCESSIBILITYCOMMUNICATION_API UPhraseTreeFunctionLibrary : public UObject
{
    GENERATED_BODY()

public:

    virtual void BindBranches(TSharedRef<FPhraseTree> PhraseTree) {};

}; 