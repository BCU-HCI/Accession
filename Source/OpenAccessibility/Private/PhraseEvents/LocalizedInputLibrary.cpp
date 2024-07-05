#include "PhraseEvents/LocalizedInputLibrary.h"
#include "PhraseEvents/Utils.h"

#include "PhraseTree/PhraseStringInputNode.h"
#include "PhraseTree/PhraseEventNode.h"

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
	PhraseTree->BindBranch(
		MakeShared<FPhraseNode>(TEXT("INPUT"), 
		TPhraseNodeArray {
		
			MakeShared<FPhraseNode>(TEXT("ADD"), 
			TPhraseNodeArray {
			
				MakeShared<FPhraseStringInputNode>(TEXT("PHRASE_TO_ADD"), 
				TPhraseNodeArray {
					
					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &ULocalizedInputLibrary::KeyboardInputAdd))

				})

			}),

			MakeShared<FPhraseNode>(TEXT("REMOVE"),
			TPhraseNodeArray {

				MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"), 
				TPhraseNodeArray {
					
					MakeShared<FPhraseEventNode>(CreateParseDelegate(this,&ULocalizedInputLibrary::KeyboardInputRemove))

				})

			}),

			MakeShared<FPhraseNode>(TEXT("RESET"), 
			TPhraseNodeArray {
			
				MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &ULocalizedInputLibrary::KeyboardInputReset))

			}),

			MakeShared<FPhraseNode>(TEXT("EXIT"), 
			TPhraseNodeArray {
				
				MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &ULocalizedInputLibrary::KeyboardInputExit))

			})

		})
	);
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

		FString FilteredText = EventUtils::RemoveWordsFromEnd(EditableText->GetText().ToString(), AmountInput->GetValue());

        EditableText->SetText(FText::FromString(FilteredText));
	}
	else if (WidgetType == "SMultiLineEditableText")
	{
        TSharedPtr<SEditableText> MultiLineEditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);
        if (!MultiLineEditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT OF TYPE - %s"), "SMultiLineEditableText");
            return;
        }

		FString FilteredText = EventUtils::RemoveWordsFromEnd(MultiLineEditableText->GetText().ToString(), AmountInput->GetValue());

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