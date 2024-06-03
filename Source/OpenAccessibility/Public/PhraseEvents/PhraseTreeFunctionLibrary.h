// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/UnrealType.h"
#include "UObject/ScriptMacros.h"

#include "PhraseTree.h"
#include "AssetAccessibilityRegistry.h"

#include "PhraseTree/Containers/ParseRecord.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"
#include "PhraseTree/Containers/Input/UParseStringInput.h"
#include "PhraseTree/Containers/Input/UParseEnumInput.h"

#include "PhraseTreeFunctionLibrary.generated.h"

// Utility Definitions

DECLARE_LOG_CATEGORY_EXTERN(LogOpenAccessibilityPhraseEvent, Log, All);

DEFINE_LOG_CATEGORY(LogOpenAccessibilityPhraseEvent);

#define GET_ACTIVE_TAB(ActiveContainerName, InActiveTabType, ...)              \
  TSharedPtr<InActiveTabType> ActiveContainerName;                             \
  {                                                                            \
    TSharedPtr<SDockTab> _AT = FGlobalTabmanager::Get()->GetActiveTab();       \
    if (!_AT.IsValid()) {                                                      \
      UE_LOG(LogOpenAccessibilityPhraseEvent, Display,                         \
             TEXT("GET_ACTIVE_TAB: NO ACTIVE TAB FOUND."));                    \
      return;                                                                  \
    }                                                                          \
                                                                               \
    ActiveContainerName = StaticCastSharedPtr<InActiveTabType>(                \
        _AT->GetContent().ToSharedPtr());                                      \
    if (!ActiveContainerName.IsValid()) {                                      \
      UE_LOG(LogOpenAccessibilityPhraseEvent, Display,                         \
             TEXT("GET_ACTIVE_TAB: CURRENT ACTIVE TAB IS NOT OF TYPE - %s"),   \
             #InActiveTabType);                                                \
      return;                                                                  \
    }                                                                          \
  };

#define GET_ACTIVE_KEYBOARD_WIDGET(ActiveContainerName, ...)                   \
  TSharedPtr<SWidget> ActiveContainerName;                                     \
  {                                                                            \
    FSlateApplication &SlateApp = FSlateApplication::Get();                    \
    if (!SlateApp.IsInitialized())                                             \
      return;                                                                  \
                                                                               \
    ActiveContainerName = SlateApp.GetKeyboardFocusedWidget();                 \
    if (!ActiveContainerName.IsValid()) {                                      \
      UE_LOG(LogOpenAccessibilityPhraseEvent, Display,                         \
             TEXT("GET_ACTIVE_KEYBOARD_WIDGET: NO ACTIVE WIDGET FOUND."));     \
      return;                                                                  \
    }                                                                          \
  };                                                                           

UCLASS(Abstract, MinimalAPI)
class UPhraseTreeFunctionLibrary : public UObject
{
    GENERATED_UCLASS_BODY()


protected:

    static TSharedRef<class FPhraseTree> GetPhraseTree();

    static TSharedRef<class FAssetAccessibilityRegistry> GetAssetRegistry();
};