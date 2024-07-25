// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "OpenAccessibility.h"
#include "OpenAccessibilityCommunication.h"

// Utility Macros

#define EMPTY_ARG

/**
 * Gets the Active Tabs Content, and stores it in the provided container name.
 * @param ActiveContainerName Name of the Container to Store the Active Tabs Content.
 * @param ReturnObject The Object to Return Upon Failure.
 */
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
  
/**
 * Gets the Active Tabs Content, and stores it in the provided container name.
 * @param ActiveContainerName Name of the Container to Store the Active Tabs Content.
 */
#define GET_ACTIVE_TAB(ActiveContainerName) \
    GET_ACTIVE_TAB_RETURN(ActiveContainerName, EMPTY_ARG)

/**
 * Gets and Casts the Active Tabs Content, and Stores it in the provided container name.
 * @param ActiveContainerName Name of the Container to Store the Active Tabs Content.
 * @param ActiveTabType Type of Widget to Cast the Active Tabs Content To.
 * @param ReturnObject The Object To Return Upon Failure.
 */
#define GET_CAST_ACTIVE_TAB_RETURN(ActiveContainerName, ActiveTabType, ReturnObject)                             \
    static_assert(TIsDerivedFrom<ActiveTabType, SWidget>::IsDerived, "Provided Type Is Not a Valid Widget Type");\
	TSharedPtr<ActiveTabType> ActiveContainerName;                                                               \
	{                                                                                                            \
		GET_ACTIVE_TAB_RETURN(_PreCastContainer, ReturnObject);                                                  \
	    ActiveContainerName = StaticCastSharedPtr<ActiveTabType>(_PreCastContainer);                             \
	    if (!ActiveContainerName.IsValid() || ActiveContainerName->GetType() != #ActiveTabType) {                \
	      UE_LOG(LogOpenAccessibilityPhraseEvent, Display,                                                       \
	             TEXT("GET_ACTIVE_TAB: FOUND ACTIVE TAB IS NOT VALID"));                                         \
	      return ReturnObject;                                                                                   \
	    }                                                                                                        \
	};

/**
 * Gets and Casts the Active Tabs Content, and Stores it in the provided container name.
 * @param ActiveContainerName Name of the Container to Store the Active Tabs Content.
 * @param ActiveTabType Type of Widget to Cast the Active Tabs Content To.
 */
#define GET_CAST_ACTIVE_TAB(ActiveContainerName, ActiveTabType) \
	GET_CAST_ACTIVE_TAB_RETURN(ActiveContainerName, ActiveTabType, EMPTY_ARG)

/**
 * Gets the Active Keyboard Widget on the Application.
 * @param ActiveContainerName Name of the Container to Store the Active Keyboard Widget.
 * @param ReturnObject The Object to Return Upon Failure.
 */
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

/**
 * Gets the Active Keyboard Widget on the Application.
 * @param ActiveContainerName Name of the Container to Store the Active Keyboard Widget
 */
#define GET_ACTIVE_KEYBOARD_WIDGET(ActiveContainerName) \
    GET_ACTIVE_KEYBOARD_WIDGET_RETURN(ActiveContainerName, EMPTY_ARG)

/**
 * Gets the Top Context Object from the Provided Parse Record.
 * @param InRecord The Record to Search for the Context Object.
 * @param ContextObjectName Name of the Container to Store the Top Context Object.
 * @param ContextObjectType Type of the Context Object.
 * @param ReturnObject The Object To Return Upon Failure.
 */
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

/**
 * Gets the Top Context Object from the Provided Parse Record.
 * @param InRecord The Record to Search for the Context Object.
 * @param ContextObjectName Name of the Container to Store the Top Context Object.
 * @param ContextObjectType Type of the Context Object.
 */
#define GET_TOP_CONTEXT(InRecord, ContextObjectName, ContextObjectType) \
    GET_TOP_CONTEXT_RETURN(InRecord, ContextObjectName, ContextObjectType, EMPTY_ARG)

// Utility Functions

/**
 * Gets the Phrase Tree from the Open AccessibilityComs Module.
 * @return A Shared Reference to the Phrase Tree.
 */
FORCEINLINE TSharedRef<FPhraseTree> GetPhraseTree() 
{
	FOpenAccessibilityCommunicationModule &OAComsModule = FOpenAccessibilityCommunicationModule::Get();

	if (OAComsModule.PhraseTree.IsValid())
		return OAComsModule.PhraseTree.ToSharedRef();

	return TSharedRef<FPhraseTree>();
}

/**
 * Gets the Asset Accessibility Registry from Open Accessibility Module.
 * @return A Shared Reference to the Asset Accessibility Registry.
 */
FORCEINLINE TSharedRef<FAssetAccessibilityRegistry> GetAssetRegistry() 
{
	FOpenAccessibilityModule &OAModule = FOpenAccessibilityModule::Get();

	if (OAModule.AssetAccessibilityRegistry.IsValid())
		return OAModule.AssetAccessibilityRegistry.ToSharedRef();

	return TSharedRef<FAssetAccessibilityRegistry>();
}

// Delegate Utilities

/**
 * Creates a Parse Event Delegate of the Provided Event.
 * @tparam ObjectType The Type of the UObject Containing the Parse Event.
 * @param ObjPtr A Pointer to the UObject Instance.
 * @param ObjFunction A Function Reference to the Parse Event on the provided UObject.
 * @return A Created UObject Based Delegate for the Parse Event.
 */
template<typename ObjectType> 
[[nodiscard]] FORCEINLINE TDelegate<void(FParseRecord&)> CreateParseDelegate(ObjectType* ObjPtr, void (ObjectType::* ObjFunction)(FParseRecord&))
{
    return TDelegate<void(FParseRecord&)>::CreateUObject(ObjPtr, ObjFunction);
}

/**
 * Creates an Input Event Delegate of the Provided Event.
 * @tparam ObjectType The Type of the UObject Containing the Input Event.
 * @tparam InputType The Type of the Input provided to the Input Event.
 * @param ObjPtr A Pointer to the UObject Instance.
 * @param ObjFunction A Function Reference to the Input Event on the provided UObject.
 * @return A Created UObject Based Delegate for the Input Event.
 */
template <typename ObjectType, typename InputType>
[[nodiscard]] FORCEINLINE TDelegate<void(InputType)> CreateInputDelegate(ObjectType* ObjPtr, void (ObjectType::* ObjFunction)(InputType))
{
	return TDelegate<void(InputType)>::CreateUObject(ObjPtr, ObjFunction);
}

/**
 * 
 * @tparam ObjectType The Type of the UObject Containing the Menu Initialization Event.
 * @param ObjPtr A Pointer to the UObject Instance.
 * @param ObjFunction A Function Reference to the Menu Initialization Event on the provided UObject.
 * @return A Created UObject Based Delegate for the Menu Initialization Event.
 */
template <typename ObjectType> 
[[nodiscard]] FORCEINLINE TDelegate<TSharedPtr<IMenu>(FParseRecord&)> CreateMenuDelegate(ObjectType* ObjPtr, TSharedPtr<IMenu> (ObjectType::* ObjFunction)(FParseRecord&))
{
	return TDelegate<TSharedPtr<IMenu>(FParseRecord&)>::CreateUObject(ObjPtr, ObjFunction);
}


// Utility Functions
namespace EventUtils
{
	/**
     * Removes the specified amount of words from the end of the provided string.
     * @param InString The String to Remove Words From.
     * @param AmountToRemove The Amount of Words to Remove.
     * @return The Newly Formatted String.
     */
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