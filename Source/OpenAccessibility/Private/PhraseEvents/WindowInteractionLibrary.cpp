#include "PhraseEvents/WindowInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "PhraseTree/PhraseEventNode.h"

UWindowInteractionLibrary::UWindowInteractionLibrary(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

UWindowInteractionLibrary::~UWindowInteractionLibrary()
{

}

void UWindowInteractionLibrary::BindBranches(TSharedRef<FPhraseTree> PhraseTree)
{
	PhraseTree->BindBranch(
		MakeShared<FPhraseNode>(TEXT("WINDOW"),
		TPhraseNodeArray{

			MakeShared<FPhraseNode>(TEXT("CLOSE"),
			TPhraseNodeArray {

				MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::CloseActiveWindow))
			}),

			MakeShared<FPhraseNode>(TEXT("TOOLBAR"), 
			TPhraseNodeArray {
					
				MakeShared<FPhraseNode>(TEXT("SELECT"), 
				TPhraseNodeArray {
					
					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SelectToolBarItem))

				})

			})

		})
	);
}

void UWindowInteractionLibrary::CloseActiveWindow(FParseRecord &Record) {
	FSlateApplication& SlateApp = FSlateApplication::Get();
	if (!SlateApp.CanDisplayWindows())
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("CloseActiveWindow: Slate Application cannot display windows."));
		return;
	}

	TSharedPtr<SWindow> ActiveWindow = SlateApp.GetActiveTopLevelWindow();
	if (!ActiveWindow.IsValid())
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("CloseActiveWindow: No Active Window Found."));
		return;
	}

	if (ActiveWindow->IsVisible() && ActiveWindow->GetTopmostAncestor().IsValid())
	{
		ActiveWindow->RequestDestroyWindow();
	}
}

void UWindowInteractionLibrary::SelectToolBarItem(FParseRecord& Record)
{
	
}
