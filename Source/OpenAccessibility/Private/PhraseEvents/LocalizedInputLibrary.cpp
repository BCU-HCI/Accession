#include "PhraseEvents/LocalizedInputLibrary.h"
#include "PhraseEvents/Utils.h"

#include "PhraseTree/Containers/Input/UParseStringInput.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"

ULocalizedInputLibrary::ULocalizedInputLibrary(const FObjectInitializer &ObjectInitializer)
{

}

ULocalizedInputLibrary::~ULocalizedInputLibrary()
{

}

void ULocalizedInputLibrary::BindBranches(TSharedRef<FPhraseTree> PhraseTree)
{

}

void ULocalizedInputLibrary::KeyboardInputAdd(FParseRecord &Record) {
	GET_ACTIVE_KEYBOARD_WIDGET(KeyboardFocusedWidget);

	FString WidgetType = KeyboardFocusedWidget->GetTypeAsString();

	UParseStringInput *PhraseInput = Record.GetPhraseInput<UParseStringInput>(TEXT("PHRASE_TO_ADD"));
    if (PhraseInput == nullptr)
		return;

	if (WidgetType == "SEditableText")
	{
		TSharedPtr<SEditableText> EditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);
		if (!EditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT OF TYPE - %s"),"SEditableText");
			return;
		}

		FString CurrText = EditableText->GetText().ToString();
        EditableText->SetText(
			FText::FromString(CurrText.TrimStartAndEnd() + TEXT(" ") + PhraseInput->GetValue())
		);
	}
	else if (WidgetType == "SMultiLineEditableText")
	{
        TSharedPtr<SEditableText> MultiLineEditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);
        if (!MultiLineEditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT OF TYPE - %s"), "SMultiLineEditableText");
            return;
        }

		FString CurrText = MultiLineEditableText->GetText().ToString();
        MultiLineEditableText->SetText(
			FText::FromString(CurrText.TrimStartAndEnd() + TEXT(" ") + PhraseInput->GetValue())
		);
	} 
	else UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT AN INTERFACEABLE TYPE"));
}

void ULocalizedInputLibrary::KeyboardInputRemove(FParseRecord& Record) 
{
	GET_ACTIVE_KEYBOARD_WIDGET(KeyboardFocusedWidget);

	FString WidgetType = KeyboardFocusedWidget->GetTypeAsString();

	UParseIntInput* AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
    if (AmountInput == nullptr)
		return;

	if (WidgetType == "SEditableText")
	{
		TSharedPtr<SEditableText> EditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);
		if (!EditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT OF TYPE - %s"),"SEditableText");
			return;
		}

		FString FilteredText = RemoveWordsFromEnd(EditableText->GetText().ToString(), AmountInput->GetValue());

        EditableText->SetText(FText::FromString(FilteredText));
	}
	else if (WidgetType == "SMultiLineEditableText")
	{
        TSharedPtr<SEditableText> MultiLineEditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);
        if (!MultiLineEditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT OF TYPE - %s"), "SMultiLineEditableText");
            return;
        }

		FString FilteredText = RemoveWordsFromEnd(MultiLineEditableText->GetText().ToString(), AmountInput->GetValue());

        MultiLineEditableText->SetText(FText::FromString(FilteredText));
	} 
	else UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT AN INTERFACEABLE TYPE"));
}

void ULocalizedInputLibrary::KeyboardInputReset(FParseRecord &Record) 
{
	GET_ACTIVE_KEYBOARD_WIDGET(KeyboardFocusedWidget);

	FString WidgetType = KeyboardFocusedWidget->GetTypeAsString();

	if (WidgetType == "SEditableText")
	{
		TSharedPtr<SEditableText> EditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);
		if (!EditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT OF TYPE - %s"),"SEditableText");
			return;
		}

		EditableText->SetText(FText::FromString(TEXT("")));
	}
	else if (WidgetType == "SMultiLineEditableText")
	{
        TSharedPtr<SEditableText> MultiLineEditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);
        if (!MultiLineEditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT OF TYPE - %s"), "SMultiLineEditableText");
            return;
        }

        MultiLineEditableText->SetText(FText::FromString(TEXT("")));
	} 
	else UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT AN INTERFACEABLE TYPE"));
}

void ULocalizedInputLibrary::KeyboardInputExit(FParseRecord &Record) 
{
	FSlateApplication& SlateApp = FSlateApplication::Get();
    if (!SlateApp.IsInitialized())
        return;

	SlateApp.ClearKeyboardFocus(EFocusCause::Cleared);
}

FString ULocalizedInputLibrary::RemoveWordsFromEnd(const FString& InString, const int32& AmountToRemove) 
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
    else
		return TEXT("");
}
