#include "PhraseEvents/LocalizedInputLibrary.h"

#include "ToolContextInterfaces.h"
#include "PhraseEvents/Utils.h"

#include "PhraseTree/PhraseStringInputNode.h"
#include "PhraseTree/PhraseEventNode.h"

#include "PhraseTree/Containers/Input/UParseStringInput.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"
#include "Widgets/Text/SMultiLineEditableText.h"

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

			MakeShared<FPhraseNode>(TEXT("CONFIRM"),
			TPhraseNodeArray {

				MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &ULocalizedInputLibrary::KeyboardInputConfirm))

			}),

			MakeShared<FPhraseNode>(TEXT("EXIT"), 
			TPhraseNodeArray {
				
				MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &ULocalizedInputLibrary::KeyboardInputExit))

			})

		})
	);
}

template<class TargetType, class WidgetType>
bool IsOfWidgetType(const TSharedRef<WidgetType>& SourceWidget)
{
	return TIsDerivedFrom<WidgetType, TargetType>::IsDerived;
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
			UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT OF TYPE - SEditableText"));
			return;
		}

		FString CurrText = EditableText->GetText().ToString();
        EditableText->SetText(
			FText::FromString(CurrText.TrimStartAndEnd() + TEXT(" ") + PhraseInput->GetValue())
		);
	}
	else if (WidgetType == "SMultiLineEditableText")
	{
        TSharedPtr<SMultiLineEditableText> MultiLineEditableText = StaticCastSharedPtr<SMultiLineEditableText>(KeyboardFocusedWidget);
        if (!MultiLineEditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT OF TYPE - SMultiLineEditableText"));
            return;
        }

		FString CurrText = MultiLineEditableText->GetText().ToString();
        MultiLineEditableText->SetText(
			FText::FromString(CurrText.TrimStartAndEnd() + TEXT(" ") + PhraseInput->GetValue())
		);
	} 
	else UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputAdd: CURRENT ACTIVE WIDGET IS NOT AN INTERFACEABLE TYPE"));
}

void ULocalizedInputLibrary::KeyboardInputRemove(FParseRecord& Record) 
{
	GET_ACTIVE_KEYBOARD_WIDGET(KeyboardFocusedWidget);

	FString WidgetType = KeyboardFocusedWidget->GetTypeAsString();

	UParseIntInput* RemoveInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
	if (RemoveInput == nullptr)
		return;

	if (WidgetType == "SEditableText")
	{
		TSharedPtr<SEditableText> EditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);
		if (!EditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputRemove: CURRENT ACTIVE WIDGET IS NOT OF TYPE - SEditableText"));
			return;
		}

		EditableText->SetText(
			FText::FromString(
				EventUtils::RemoveWordsFromEnd(EditableText->GetText().ToString(), RemoveInput->GetValue())
			)
		);
	}
	else if (WidgetType == "SMultiLineEditableText")
	{
		TSharedPtr<SMultiLineEditableText> MultiLineEditableText = StaticCastSharedPtr<SMultiLineEditableText>(KeyboardFocusedWidget);
		if (!MultiLineEditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputRemove: CURRENT ACTIVE WIDGET IS NOT OF TYPE - SMultiLineEditableText"));
			return;
		}

		MultiLineEditableText->SetText(
			FText::FromString(
				EventUtils::RemoveWordsFromEnd(MultiLineEditableText->GetText().ToString(), RemoveInput->GetValue())
			)
		);
	}
	else UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputRemove: CURRENT ACTIVE WIDGET IS NOT AN INTERFACEABLE TYPE"));
}

void ULocalizedInputLibrary::KeyboardInputReset(FParseRecord &Record) 
{
	GET_ACTIVE_KEYBOARD_WIDGET(KeyboardFocusedWidget);

	FString WidgetType = KeyboardFocusedWidget->GetTypeAsString();

	if (WidgetType == "SEditableText")
	{
		TSharedPtr<SEditableText> EditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);
		if (!EditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputReset: CURRENT ACTIVE WIDGET IS NOT OF TYPE - SEditableText"));
			return;
		}

		EditableText->SetText(
			FText::FromString(TEXT(""))
		);
	}
	else if (WidgetType == "SMultiLineEditableText")
	{
		TSharedPtr<SMultiLineEditableText> MultiLineEditableText = StaticCastSharedPtr<SMultiLineEditableText>(KeyboardFocusedWidget);
		if (!MultiLineEditableText.IsValid()) {
			UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputReset: CURRENT ACTIVE WIDGET IS NOT OF TYPE - SMultiLineEditableText"));
			return;
		}

		MultiLineEditableText->SetText(
			FText::FromString(TEXT(""))
		);
	}
	else UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputReset: CURRENT ACTIVE WIDGET IS NOT AN INTERFACEABLE TYPE"));
}

void ULocalizedInputLibrary::KeyboardInputConfirm(FParseRecord& Record)
{
	GET_ACTIVE_KEYBOARD_WIDGET(KeyboardFocusedWidget);

	/*
	if (WidgetType == SEditableText::StaticWidgetClass().GetWidgetType())
	{
		TSharedPtr<SEditableText> EditableText = StaticCastSharedPtr<SEditableText>(KeyboardFocusedWidget);
		if (!EditableText.IsValid())
		{
			UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputConfirm: CURRENT ACTIVE WIDGET IS NOT OF TYPE - SEditableText"))
			return;
		}

	}
	else if (WidgetType == SMultiLineEditableText::StaticWidgetClass().GetWidgetType())
	{
		TSharedPtr<SMultiLineEditableText> MultiLineEditableText = StaticCastSharedPtr<SMultiLineEditableText>(KeyboardFocusedWidget);
		if (!MultiLineEditableText.IsValid())
		{
			UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputConfirm: CURRENT ACTIVE WIDGET IS NOT OF TYPE - SMultiLineEditableText"))
			return;
		}

		

	}
	else UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("KeyboardInputConfirm: CURRENT ACTIVE WIDGET IS NOT AN INTERFACEABLE TYPE"))
	*/
}

void ULocalizedInputLibrary::KeyboardInputExit(FParseRecord &Record) 
{
	FSlateApplication& SlateApp = FSlateApplication::Get();
    if (!SlateApp.IsInitialized())
        return;

	SlateApp.ClearKeyboardFocus();
}