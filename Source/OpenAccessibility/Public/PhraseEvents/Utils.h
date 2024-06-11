// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "OpenAccessibility.h"
#include "OpenAccessibilityCommunication.h"

// Utility Macros

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
    ActiveContainerName =                                                      \
        StaticCastSharedPtr<InActiveTabType>(_AT->GetContent().ToSharedPtr()); \
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

#define GET_TOP_CONTEXT(InRecord, ContextObjectName, ContextObjectType, ...)   \
  ContextObjectType *ContextObjectName;                                        \
  {                                                                            \
    ContextObjectName = InRecord.GetContextObj<ContextObjectType>();           \
    if (ContextObjectName == nullptr) {                                        \
      UE_LOG(LogOpenAccessibilityPhraseEvent, Display,                         \
             TEXT("GET_TOP_CONTEXT: NO CONTEXT OBJECT FOUND."))                \
      return;                                                                  \
    }                                                                          \
  };

// Utility Functions

TSharedRef<FPhraseTree> GetPhraseTree() 
{
  FOpenAccessibilityCommunicationModule &OAComsModule =
      FOpenAccessibilityCommunicationModule::Get();

  if (OAComsModule.PhraseTree.IsValid())
    return OAComsModule.PhraseTree.ToSharedRef();
  else
    return TSharedRef<FPhraseTree>();
}

TSharedRef<FAssetAccessibilityRegistry> GetAssetRegistry() 
{
  FOpenAccessibilityModule &OAModule = FOpenAccessibilityModule::Get();

  if (OAModule.AssetAccessibilityRegistry.IsValid())
    return OAModule.AssetAccessibilityRegistry.ToSharedRef();
  else
    return TSharedRef<FAssetAccessibilityRegistry>();

  return TSharedRef<FAssetAccessibilityRegistry>();
}

// Delegate Utilities

template<typename ObjectType> 
[[nodiscard]] FORCEINLINE TDelegate<void(FParseRecord&)> CreateParseDelegate(ObjectType* ObjPtr, void (ObjectType::* ObjFunction)(FParseRecord&))
{
    return TDelegate<void(FParseRecord &)>::CreateUObject(ObjPtr, ObjFunction);
}

template <typename ObjectType, typename InputType>
[[nodiscard]] FORCEINLINE TDelegate<void(InputType)> CreateInputDelegate(ObjectType* ObjPtr, void (ObjectType::* ObjFunction)(InputType))
{
	return TDelegate<void(InputType)>::CreateUObject(ObjPtr, ObjFunction);
}

template <typename ObjectType> 
[[nodiscard]] FORCEINLINE TDelegate<TSharedPtr<IMenu>(FParseRecord&)> CreateMenuDelegate(ObjectType* ObjPtr, TSharedPtr<IMenu> (ObjectType::* ObjFunction)(FParseRecord&))
{
	return TDelegate<TSharedPtr<IMenu>(FParseRecord&)>::CreateUObject(ObjPtr, ObjFunction);
}
