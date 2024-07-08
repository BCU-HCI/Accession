// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "OpenAccessibility.h"
#include "OpenAccessibilityCommunication.h"

// Utility Macros

#define EMPTY_ARG

#define GET_ACTIVE_TAB_RETURN(ActiveContainerName, ReturnObject)                \
  TSharedPtr<SWidget> ActiveContainerName;                                      \
  {                                                                             \
    TSharedPtr<SDockTab> _AT = FGlobalTabmanager::Get()->GetActiveTab();        \
    if (_AT == nullptr || !_AT.IsValid()) {                                     \
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display,                        \
			TEXT("GET_ACTIVE_TAB: NO ACTIVE TAB FOUND"));                       \
      return ReturnObject;                                                      \
    }                                                                           \
																				\
    ActiveContainerName = _AT->GetContent();                                    \
    if (_AT == nullptr || !ActiveContainerName.IsValid()) {                     \
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display,                        \
			TEXT("GET_ACTIVE_TAB: FOUND ACTIVE TAB IS NOT VALID"));             \
		return ReturnObject;                                                    \
    }                                                                           \
  };

#define GET_ACTIVE_TAB(ActiveContainerName) \
    GET_ACTIVE_TAB_RETURN(ActiveContainerName, EMPTY_ARG)

#define GET_CAST_ACTIVE_TAB_RETURN(ActiveContainerName, ActiveTabType, ReturnObject)                \
	TSharedPtr<ActiveTabType> ActiveContainerName;                                                  \
	{                                                                                               \
		GET_ACTIVE_TAB_RETURN(_PreCastContainer, ReturnObject);                                     \
	    ActiveContainerName = StaticCastSharedPtr<ActiveTabType>(_PreCastContainer);                \
	    if (!ActiveContainerName.IsValid() || ActiveContainerName->GetType() != #ActiveTabType) {   \
	      UE_LOG(LogOpenAccessibilityPhraseEvent, Display,                                          \
	             TEXT("GET_ACTIVE_TAB: FOUND ACTIVE TAB IS NOT VALID"));                            \
	      return ReturnObject;                                                                      \
	    }                                                                                           \
	};

#define GET_CAST_ACTIVE_TAB(ActiveContainerName, ActiveTabType) \
	GET_CAST_ACTIVE_TAB_RETURN(ActiveContainerName, ActiveTabType, EMPTY_ARG)

#define GET_ACTIVE_KEYBOARD_WIDGET_RETURN(ActiveContainerName, ReturnObject)   \
  TSharedPtr<SWidget> ActiveContainerName;                                     \
  {                                                                            \
    FSlateApplication &SlateApp = FSlateApplication::Get();                    \
    if (!SlateApp.IsInitialized())                                             \
      return ReturnObject;                                                     \
                                                                               \
    ActiveContainerName = SlateApp.GetKeyboardFocusedWidget();                 \
    if (!ActiveContainerName.IsValid()) {                                      \
      UE_LOG(LogOpenAccessibilityPhraseEvent, Display,                         \
             TEXT("GET_ACTIVE_KEYBOARD_WIDGET: NO ACTIVE WIDGET FOUND."));     \
      return ReturnObject;                                                     \
    }                                                                          \
  };

#define GET_ACTIVE_KEYBOARD_WIDGET(ActiveContainerName) \
    GET_ACTIVE_KEYBOARD_WIDGET_RETURN(ActiveContainerName, EMPTY_ARG)

#define GET_TOP_CONTEXT_RETURN(InRecord, ContextObjectName, ContextObjectType, ReturnObject) \
  ContextObjectType *ContextObjectName;                                                      \
  {                                                                                          \
    ContextObjectName = InRecord.GetContextObj<ContextObjectType>();                         \
    if (ContextObjectName == nullptr) {                                                      \
      UE_LOG(LogOpenAccessibilityPhraseEvent, Display,                                       \
             TEXT("GET_TOP_CONTEXT: NO CONTEXT OBJECT FOUND."))                              \
      return ReturnObject;                                                                   \
    }                                                                                        \
  };

#define GET_TOP_CONTEXT(InRecord, ContextObjectName, ContextObjectType) \
    GET_TOP_CONTEXT_RETURN(InRecord, ContextObjectName, ContextObjectType, EMPTY_ARG)

// Utility Functions

FORCEINLINE TSharedRef<FPhraseTree> GetPhraseTree() 
{
	FOpenAccessibilityCommunicationModule &OAComsModule = FOpenAccessibilityCommunicationModule::Get();

	if (OAComsModule.PhraseTree.IsValid())
		return OAComsModule.PhraseTree.ToSharedRef();

	return TSharedRef<FPhraseTree>();
}

FORCEINLINE TSharedRef<FAssetAccessibilityRegistry> GetAssetRegistry() 
{
	FOpenAccessibilityModule &OAModule = FOpenAccessibilityModule::Get();

	if (OAModule.AssetAccessibilityRegistry.IsValid())
		return OAModule.AssetAccessibilityRegistry.ToSharedRef();

	return TSharedRef<FAssetAccessibilityRegistry>();
}

// Delegate Utilities

template<typename ObjectType> 
[[nodiscard]] FORCEINLINE TDelegate<void(FParseRecord&)> CreateParseDelegate(ObjectType* ObjPtr, void (ObjectType::* ObjFunction)(FParseRecord&))
{
    return TDelegate<void(FParseRecord&)>::CreateUObject(ObjPtr, ObjFunction);
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


// Utility Functions
namespace EventUtils
{
    [[nodiscard]] FORCEINLINE FString RemoveWordsFromEnd(const FString& InString, const int32& AmountToRemove)
    {
        TArray<FString> SplitTextBoxString;
        InString.ParseIntoArrayWS(SplitTextBoxString);

        int RemovedAmount = 0;
        int CurrentIndex = SplitTextBoxString.Num() - 1;
        while (RemovedAmount < AmountToRemove) {
            if (SplitTextBoxString.IsEmpty())
                break;

            SplitTextBoxString.RemoveAt(CurrentIndex--);
            RemovedAmount++;
        }

        if (SplitTextBoxString.Num() > 0)
            return FString::Join(SplitTextBoxString, TEXT(" "));

        return TEXT("");
    }
}