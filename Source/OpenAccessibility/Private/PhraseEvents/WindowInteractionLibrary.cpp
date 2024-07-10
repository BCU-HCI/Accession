#include "PhraseEvents/WindowInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseEventNode.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"

#include "AccessibilityWrappers/AccessibilityWindowToolbar.h"

UWindowInteractionLibrary::UWindowInteractionLibrary(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	WindowToolBar = NewObject<UAccessibilityWindowToolbar>();
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
					
					MakeShared<FPhraseInputNode<int32>>(TEXT("ITEM_INDEX"), 
					TPhraseNodeArray {
						
						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SelectToolBarItem))
					
					})

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

	TSharedPtr<SWindow> RootWindow = FGlobalTabmanager::Get()->GetRootWindow();
	if (ActiveWindow->IsVisible() && ActiveWindow != RootWindow)
	{
		ActiveWindow->RequestDestroyWindow();
	}
}

void UWindowInteractionLibrary::SelectToolBarItem(FParseRecord& Record)
{
	UParseIntInput* ItemIndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("ITEM_INDEX"));
	if (ItemIndexInput == nullptr) 
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("SelectToolBarItem: No Item Index Found."));
		return;
	}

	WindowToolBar->SelectToolbarItem(ItemIndexInput->GetValue());
}
