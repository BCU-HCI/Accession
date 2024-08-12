#include "PhraseEvents/WindowInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseEventNode.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"

#include "AccessibilityWrappers/AccessibilityWindowToolbar.h"
#include "Framework/Docking/TabManager.h"

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
	PhraseTree->BindBranches(
		TPhraseNodeArray{

			MakeShared<FPhraseNode>(TEXT("WINDOW"),
			TPhraseNodeArray{

				MakeShared<FPhraseNode>(TEXT("NEXT"),
				TPhraseNodeArray {

					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SwitchNextActiveWindow))

				}),

				MakeShared<FPhraseNode>(TEXT("PREVIOUS"),
				TPhraseNodeArray {

					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SwitchPrevActiveWindow))

				}),

				MakeShared<FPhraseNode>(TEXT("CLOSE"),
				TPhraseNodeArray {

					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::CloseActiveWindow))

				}),

			}),

			MakeShared<FPhraseNode>(TEXT("TAB"),
			TPhraseNodeArray {

				MakeShared<FPhraseNode>(TEXT("NEXT"),
				TPhraseNodeArray {

					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SwitchNextTabInStack))

				}),

				MakeShared<FPhraseNode>(TEXT("PREVIOUS"),
				TPhraseNodeArray {

					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SwitchPrevTabInStack))

				})

			}),

			MakeShared<FPhraseNode>(TEXT("TOOLBAR"),
			TPhraseNodeArray {

				MakeShared<FPhraseInputNode<int32>>(TEXT("ITEM_INDEX"),
				TPhraseNodeArray {

					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SelectToolBarItem))

				})

			}),

		}
	);
}

void UWindowInteractionLibrary::SwitchNextActiveWindow(FParseRecord& Record)
{
	GET_ACTIVE_REGULAR_WINDOW(ActiveWindow)

	TArray<TSharedRef<SWindow>> AllWindows;
	FSlateApplication::Get().GetAllVisibleWindowsOrdered(AllWindows);

	int32 FoundIndex;
	if (!AllWindows.Find(ActiveWindow.ToSharedRef(), FoundIndex))
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("SwitchNextActiveWindow: Cannot Find the Current Active Window."))
		return;
	}

	TSharedRef<SWindow> NextWindow = AllWindows[FoundIndex + 1 % AllWindows.Num()];

	NextWindow->BringToFront(true);

	// Set Window Major Tab Focus?
}

void UWindowInteractionLibrary::SwitchPrevActiveWindow(FParseRecord& Record)
{
	GET_ACTIVE_REGULAR_WINDOW(ActiveWindow)

	TArray<TSharedRef<SWindow>> AllWindows;
	FSlateApplication::Get().GetAllVisibleWindowsOrdered(AllWindows);

	int32 FoundIndex;
	if (!AllWindows.Find(ActiveWindow.ToSharedRef(), FoundIndex))
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("SwitchPrevActiveWindow: Cannot Find the Current Active Window."))
			return;
	}

	TSharedRef<SWindow> PrevWindow = AllWindows[
		FoundIndex - 1 < 0
			? AllWindows.Num() - 1
			: FoundIndex - 1
	];

	PrevWindow->BringToFront(true);

	// Set Window Major Tab Focus?
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

namespace TabUtils
{
	class FOpenArea : public FTabManager::FArea
	{
	public:
		const TArray<TSharedRef<FLayoutNode>>& GetChildNodes()
		{
			return ChildNodes;
		}
	};

	class FOpenStack : public FTabManager::FStack
	{
	public:
		const TArray<FTabManager::FTab> GetTabs()
		{
			return Tabs;
		}
	};

	class FOpenSplitter : public FTabManager::FSplitter
	{
	public:
		const TArray<TSharedRef<FLayoutNode>>& GetChildNodes()
		{
			return ChildNodes;
		}
		
	};

	[[nodiscard]] TArray<FTabManager::FTab> CollectManagedTabs(const TSharedRef<FTabManager>& TabManager)
	{
		TArray<FTabManager::FTab> ManagedTabs = TArray<FTabManager::FTab>();

		const TSharedRef<FTabManager::FLayout> Layout = TabManager->PersistLayout();
		const TSharedPtr<FOpenArea> MainArea = StaticCastSharedPtr<FOpenArea>(Layout->GetPrimaryArea().Pin());
		if (!MainArea.IsValid())
		{
			UE_LOG(LogOpenAccessibility, Warning, TEXT("CollectManagedTabs: Primary Area is Not Valid"))
			return ManagedTabs;
		}

		TSharedPtr<FTabManager::FLayoutNode> CurrentNode = nullptr;
		TArray<TSharedRef<FTabManager::FLayoutNode>> NodesToCheck = TArray{
			MainArea->GetChildNodes()
		};

		while(!NodesToCheck.IsEmpty())
		{
			CurrentNode = NodesToCheck[0];
			NodesToCheck.RemoveAt(0);

			if (TSharedPtr<FTabManager::FStack> AsStack = CurrentNode->AsStack())
			{
				ManagedTabs.Append(StaticCastSharedPtr<FOpenStack>(AsStack)->GetTabs());
			}
			else if (TSharedPtr<FTabManager::FArea> AsArea = CurrentNode->AsArea())
			{
				NodesToCheck.Append(StaticCastSharedPtr<FOpenArea>(AsArea)->GetChildNodes());
			}
			else if (TSharedPtr<FTabManager::FSplitter> AsSplitter = CurrentNode->AsSplitter())
			{
				NodesToCheck.Append(StaticCastSharedPtr<FOpenArea>(AsSplitter)->GetChildNodes());
			}
			else
			{
				UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("CollectManagedTabs: Unknown Node Type."))
			}
		}

		return ManagedTabs;
	}
}

void UWindowInteractionLibrary::SwitchNextTabInStack(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveTab);

	TSharedPtr<FTabManager> ActiveTabManager = ActiveTab->GetTabManagerPtr();
	if (!ActiveTabManager.IsValid())
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("SwitchNextTabInStack: Cannot Find Active Tab Manager"))
		return;
	}

	TArray<FTabManager::FTab> FoundTabs = TabUtils::CollectManagedTabs(ActiveTabManager.ToSharedRef());

	const FTabId ActiveTabId = ActiveTab->GetLayoutIdentifier();
	for (int32 i = 0; i < FoundTabs.Num(); i++)
	{
		if (FoundTabs[i].TabId == ActiveTabId)
		{
			const FTabManager::FTab NextTab = FoundTabs[i + 1 % FoundTabs.Num()];

			const TSharedPtr<SDockTab> NextTabWidget = ActiveTabManager->FindExistingLiveTab(
				NextTab.TabId
			);

			if (!NextTabWidget.IsValid())
			{
				UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("SwitchNextTabInStack: Found Next Tab is Invalid."))
				return;
			}

			FGlobalTabmanager::Get()->SetActiveTab(NextTabWidget);
			NextTabWidget->ActivateInParent(SetDirectly);

			break;
		}
	}

	/*
	// Most Straightforward Implementation, But Requires Private Core Tab Classes
	
	TSharedPtr<SDockingTabStack> ActiveTabStack = ActiveTab->GetParentDockTabStack();
	if (!ActiveTabStack.IsValid())
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("SwitchNextTabInStack: No Parent Tab Stack Found."))
		return;
	}

	TArray<TSharedRef<SDockTab>> AllTabs = ActiveTabStack->GetAllChildTabs();

	int32 FoundIndex;
	if (!AllTabs.Find(ActiveTab.ToSharedRef(), FoundIndex))
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("SwitchNextTabInStack: Active Tab Not Found In Tab Stack."))
		return;
	}

	TSharedRef<SDockTab> NextTab = AllTabs[FoundIndex + 1 % AllTabs.Num()];

	FGlobalTabmanager::Get()->SetActiveTab(NextTab);
	NextTab->ActivateInParent(SetDirectly);
	*/
}

void UWindowInteractionLibrary::SwitchPrevTabInStack(FParseRecord& Record)
{
	GET_ACTIVE_TAB(ActiveTab);

	TSharedPtr<FTabManager> ActiveTabManager = ActiveTab->GetTabManagerPtr();
	if (!ActiveTabManager.IsValid())
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("SwitchPrevTabInStack: Cannot Find Active Tab Manager"))
			return;
	}

	TArray<FTabManager::FTab> FoundTabs = TabUtils::CollectManagedTabs(ActiveTabManager.ToSharedRef());

	const FTabId ActiveTabId = ActiveTab->GetLayoutIdentifier();
	for (int32 i = 0; i < FoundTabs.Num(); i++)
	{
		if (FoundTabs[i].TabId == ActiveTabId)
		{
			const FTabManager::FTab NextTab = FoundTabs[
				i - 1 < 0
				? FoundTabs.Num() - 1
				: i - 1
			];

			const TSharedPtr<SDockTab> NextTabWidget = ActiveTabManager->FindExistingLiveTab(
				NextTab.TabId
			);

			if (!NextTabWidget.IsValid())
			{
				UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("SwitchNextTabInStack: Found Next Tab is Invalid."))
					return;
			}

			FGlobalTabmanager::Get()->SetActiveTab(NextTabWidget);
			NextTabWidget->ActivateInParent(SetDirectly);

			break;
		}
	}

	/*
	// Most Straightforward Implementation, But Requires Private Core Tab Classes

	TSharedPtr<SDockingTabStack> ActiveTabStack = ActiveTab->GetParentDockTabStack();
	if (!ActiveTabStack.IsValid())
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("SwitchPrevTabInStack: No Parent Tab Stack Found."))
			return;
	}

	TArray<TSharedRef<SDockTab>> AllTabs = ActiveTabStack->GetAllChildTabs();

	int32 FoundIndex;
	if (!AllTabs.Find(ActiveTab.ToSharedRef(), FoundIndex))
	{
		UE_LOG(LogOpenAccessibilityPhraseEvent, Warning, TEXT("SwitchPrevTabInStack: Active Tab Not Found In Tab Stack."))
			return;
	}

	TSharedRef<SDockTab> PrevTab = AllTabs[
		FoundIndex - 1 < 0
			? AllTabs.Num() - 1
			: FoundIndex - 1
	];

	FGlobalTabmanager::Get()->SetActiveTab(PrevTab);
	PrevTab->ActivateInParent(SetDirectly);
	*/
}
