// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "PhraseEvents/WindowInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseEventNode.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"
#include "PhraseTree/Containers/Input/UParseStringInput.h"

#include "FunctionalityWrappers/WindowToolbarIndex.h"

#include "Algo/LevenshteinDistance.h"
#include "Framework/Docking/TabManager.h"

#include "PhraseTree/PhraseIntInputNode.h"
#include "PhraseTree/PhraseStringInputNode.h"

UWindowInteractionLibrary::UWindowInteractionLibrary(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	WindowToolBar = NewObject<UWindowToolbarIndex>();
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
																TPhraseNodeArray{

																	MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SwitchNextActiveWindow))

																}),

										MakeShared<FPhraseNode>(TEXT("PREVIOUS"),
																TPhraseNodeArray{

																	MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SwitchPrevActiveWindow))

																}),

										MakeShared<FPhraseNode>(TEXT("CLOSE"),
																TPhraseNodeArray{

																	MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::CloseActiveWindow))

																}),

									}),

			MakeShared<FPhraseNode>(TEXT("TAB"),
									TPhraseNodeArray{

										MakeShared<FPhraseNode>(TEXT("NEXT"),
																TPhraseNodeArray{

																	MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SwitchNextTabInStack))

																}),

										MakeShared<FPhraseNode>(TEXT("PREVIOUS"),
																TPhraseNodeArray{

																	MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SwitchPrevTabInStack))

																}),

										MakeShared<FPhraseNode>(TEXT("SELECT"),
																TPhraseNodeArray{

																	MakeShared<FPhraseStringInputNode>(TEXT("TAB_NAME"),
																									   TPhraseNodeArray{

																										   MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SelectTabInStack))

																									   })

																})}),

			MakeShared<FPhraseNode>(TEXT("TOOLBAR"),
									TPhraseNodeArray{

										MakeShared<FPhraseIntInputNode>(TEXT("ITEM_INDEX"),
																		TPhraseNodeArray{

																			MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::SelectToolBarItem))

																		})

									}),

			MakeShared<FPhraseNode>(TEXT("UNDO"),
									TPhraseNodeArray{

										MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::UndoAction))

									}),

			MakeShared<FPhraseNode>(TEXT("REDO"),
									TPhraseNodeArray{

										MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UWindowInteractionLibrary::RedoAction))

									})});
}

void UWindowInteractionLibrary::SwitchNextActiveWindow(FParseRecord &Record)
{
	GET_ACTIVE_REGULAR_WINDOW(ActiveWindow)

	TArray<TSharedRef<SWindow>> AllWindows;
	FSlateApplication::Get().GetAllVisibleWindowsOrdered(AllWindows);

	int32 FoundIndex;
	if (!AllWindows.Find(ActiveWindow.ToSharedRef(), FoundIndex))
	{
		UE_LOG(LogAccessionPhraseEvent, Display, TEXT("SwitchNextActiveWindow: Cannot Find the Current Active Window."))
		return;
	}

	TSharedRef<SWindow> NextWindow = AllWindows[FoundIndex + 1 % AllWindows.Num()];

	NextWindow->BringToFront(true);

	// Set Window Major Tab Focus?
}

void UWindowInteractionLibrary::SwitchPrevActiveWindow(FParseRecord &Record)
{
	GET_ACTIVE_REGULAR_WINDOW(ActiveWindow)

	TArray<TSharedRef<SWindow>> AllWindows;
	FSlateApplication::Get().GetAllVisibleWindowsOrdered(AllWindows);

	int32 FoundIndex;
	if (!AllWindows.Find(ActiveWindow.ToSharedRef(), FoundIndex))
	{
		UE_LOG(LogAccessionPhraseEvent, Display, TEXT("SwitchPrevActiveWindow: Cannot Find the Current Active Window."))
		return;
	}

	TSharedRef<SWindow> PrevWindow = AllWindows[FoundIndex - 1 < 0
													? AllWindows.Num() - 1
													: FoundIndex - 1];

	PrevWindow->BringToFront(true);

	// Set Window Major Tab Focus?
}

void UWindowInteractionLibrary::CloseActiveWindow(FParseRecord &Record)
{
	FSlateApplication &SlateApp = FSlateApplication::Get();
	if (!SlateApp.CanDisplayWindows())
	{
		UE_LOG(LogAccessionPhraseEvent, Display, TEXT("CloseActiveWindow: Slate Application cannot display windows."));
		return;
	}

	TSharedPtr<SWindow> ActiveWindow = SlateApp.GetActiveTopLevelWindow();
	if (!ActiveWindow.IsValid())
	{
		UE_LOG(LogAccessionPhraseEvent, Display, TEXT("CloseActiveWindow: No Active Window Found."));
		return;
	}

	TSharedPtr<SWindow> RootWindow = FGlobalTabmanager::Get()->GetRootWindow();
	if (ActiveWindow->IsVisible() && ActiveWindow != RootWindow)
	{
		ActiveWindow->RequestDestroyWindow();
	}
}

void UWindowInteractionLibrary::SelectToolBarItem(FParseRecord &Record)
{
	UParseIntInput *ItemIndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("ITEM_INDEX"));
	if (ItemIndexInput == nullptr)
	{
		UE_LOG(LogAccessionPhraseEvent, Display, TEXT("SelectToolBarItem: No Item Index Found."));
		return;
	}

	WindowToolBar->SelectToolbarItem(ItemIndexInput->GetValue());
}

/**
 * [HACK] Utilities for Interacting with the Tab Manager Class.
 */
namespace TabUtils
{

	/**
	 * Derived Area Class to Access Child Nodes, through Dynamic Casting.
	 */
	class FOpenArea : public FTabManager::FArea
	{
	public:
		/**
		 * Gets a Reference to the Areas Child Nodes Array.
		 * @return A Reference to the FAreas Child Layout Nodes.
		 */
		const TArray<TSharedRef<FLayoutNode>> &GetChildNodes()
		{
			return ChildNodes;
		}
	};

	/**
	 * Derived Stack Class to Access Tabs, through Dynamic Casting.
	 */
	class FOpenStack : public FTabManager::FStack
	{
	public:
		/**
		 * Gets a Reference to the Stacks Child Tabs Array.
		 * @return A Reference to the FStacks Child Tabs.
		 */
		const TArray<FTabManager::FTab> &GetTabs()
		{
			return Tabs;
		}
	};

	/**
	 * Derived Splitter Class to Access Child Nodes, through Dynamic Casting.
	 */
	class FOpenSplitter : public FTabManager::FSplitter
	{
	public:
		/**
		 * Gets a Reference to the Splitters Child Nodes Array.
		 * @return A Reference to the FSplitter Child Layout Nodes.
		 */
		const TArray<TSharedRef<FLayoutNode>> &GetChildNodes()
		{
			return ChildNodes;
		}
	};

	/**
	 * Collects all Tabs Manager by this Tab Manager, through its Layout.
	 * @param TabManager The Tab Manager to Collect Descendant Tabs from.
	 * @return An Array of Found Tabs from the Root Layout.
	 */
	[[nodiscard]] TArray<FTabManager::FTab> CollectManagedTabs(const TSharedRef<FTabManager> &TabManager)
	{
		TArray<FTabManager::FTab> ManagedTabs = TArray<FTabManager::FTab>();

		const TSharedRef<FTabManager::FLayout> Layout = TabManager->PersistLayout();
		const TSharedPtr<FOpenArea> MainArea = StaticCastSharedPtr<FOpenArea>(Layout->GetPrimaryArea().Pin());
		if (!MainArea.IsValid())
		{
			UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("CollectManagedTabs: Primary Area is Not Valid"))
			return ManagedTabs;
		}

		TSharedPtr<FTabManager::FLayoutNode> CurrentNode = nullptr;
		TArray<TSharedRef<FTabManager::FLayoutNode>> NodesToCheck = TArray{
			MainArea->GetChildNodes()};

		while (!NodesToCheck.IsEmpty())
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
				UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("CollectManagedTabs: Unknown Node Type."))
			}
		}

		return ManagedTabs;
	}
}

void UWindowInteractionLibrary::SwitchNextTabInStack(FParseRecord &Record)
{
	GET_ACTIVE_TAB(ActiveTab);

	TSharedPtr<FTabManager> ActiveTabManager = ActiveTab->GetTabManagerPtr();
	if (!ActiveTabManager.IsValid())
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SwitchNextTabInStack: Cannot Find Active Tab Manager"))
		return;
	}

	TArray<FTabManager::FTab> FoundTabs = TabUtils::CollectManagedTabs(ActiveTabManager.ToSharedRef());

	const FTabId ActiveTabId = ActiveTab->GetLayoutIdentifier();
	for (int32 i = 0; i < FoundTabs.Num(); i++)
	{
		if (FoundTabs[i].TabId == ActiveTabId)
		{
			TSharedPtr<SDockTab> NextTabWidget = TSharedPtr<SDockTab>();

			int CurrentTabIndex = i;
			while (!NextTabWidget.IsValid())
			{
				CurrentTabIndex += 1;
				CurrentTabIndex %= FoundTabs.Num();

				const FTabManager::FTab NextTab = FoundTabs[CurrentTabIndex];

				if (NextTab.TabId == ActiveTabId)
				{
					UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SwitchNextTabInStack: No Next Tab Found."))
					return;
				}

				NextTabWidget = ActiveTabManager->FindExistingLiveTab(
					NextTab.TabId);
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
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SwitchNextTabInStack: No Parent Tab Stack Found."))
		return;
	}

	TArray<TSharedRef<SDockTab>> AllTabs = ActiveTabStack->GetAllChildTabs();

	int32 FoundIndex;
	if (!AllTabs.Find(ActiveTab.ToSharedRef(), FoundIndex))
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SwitchNextTabInStack: Active Tab Not Found In Tab Stack."))
		return;
	}

	TSharedRef<SDockTab> NextTab = AllTabs[FoundIndex + 1 % AllTabs.Num()];

	FGlobalTabmanager::Get()->SetActiveTab(NextTab);
	NextTab->ActivateInParent(SetDirectly);
	*/
}

void UWindowInteractionLibrary::SwitchPrevTabInStack(FParseRecord &Record)
{
	GET_ACTIVE_TAB(ActiveTab);

	TSharedPtr<FTabManager> ActiveTabManager = ActiveTab->GetTabManagerPtr();
	if (!ActiveTabManager.IsValid())
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SwitchPrevTabInStack: Cannot Find Active Tab Manager"))
		return;
	}

	TArray<FTabManager::FTab> FoundTabs = TabUtils::CollectManagedTabs(ActiveTabManager.ToSharedRef());
	if (FoundTabs.IsEmpty())
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SwitchPrevTabInStack: No Tabs Found."))
		return;
	}

	const FTabId ActiveTabId = ActiveTab->GetLayoutIdentifier();
	for (int32 i = 0; i < FoundTabs.Num(); i++)
	{
		if (FoundTabs[i].TabId == ActiveTabId)
		{
			TSharedPtr<SDockTab> PrevTabWidget = TSharedPtr<SDockTab>();

			int CurrentTabIndex = i;
			while (!PrevTabWidget.IsValid())
			{
				CurrentTabIndex -= 1;
				if (CurrentTabIndex < 0)
				{
					CurrentTabIndex = FoundTabs.Num() - 1;
				}

				const FTabManager::FTab NextTab = FoundTabs[CurrentTabIndex];

				if (NextTab.TabId == ActiveTabId)
				{
					UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SwitchNextTabInStack: No Next Tab Found."))
					return;
				}

				PrevTabWidget = ActiveTabManager->FindExistingLiveTab(
					NextTab.TabId);
			}

			FGlobalTabmanager::Get()->SetActiveTab(PrevTabWidget);
			PrevTabWidget->ActivateInParent(SetDirectly);

			break;
		}
	}

	/*
	// Most Straightforward Implementation, But Requires Private Core Tab Classes

	TSharedPtr<SDockingTabStack> ActiveTabStack = ActiveTab->GetParentDockTabStack();
	if (!ActiveTabStack.IsValid())
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SwitchPrevTabInStack: No Parent Tab Stack Found."))
			return;
	}

	TArray<TSharedRef<SDockTab>> AllTabs = ActiveTabStack->GetAllChildTabs();

	int32 FoundIndex;
	if (!AllTabs.Find(ActiveTab.ToSharedRef(), FoundIndex))
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SwitchPrevTabInStack: Active Tab Not Found In Tab Stack."))
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

void UWindowInteractionLibrary::SelectTabInStack(FParseRecord &Record)
{
	UParseStringInput *TabNameInput = Record.GetPhraseInput<UParseStringInput>(TEXT("TAB_NAME"));
	if (TabNameInput == nullptr)
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SelectTabInStack: No Tab Name Input Found."))
		return;
	}

	GET_ACTIVE_TAB(ActiveTab);

	TSharedPtr<FTabManager> ActiveTabManager = ActiveTab->GetTabManagerPtr();
	if (!ActiveTabManager.IsValid())
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SelectTabInStack: Cannot Find Active Tab Manager"))
		return;
	}

	TArray<FTabManager::FTab> FoundTabs = TabUtils::CollectManagedTabs(ActiveTabManager.ToSharedRef());
	if (FoundTabs.IsEmpty())
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SelectTabInStack: No Tabs Found."))
		return;
	}

	FString TargetTabName = TabNameInput->GetValue();

	TSharedPtr<SDockTab> FoundTabWidget = TSharedPtr<SDockTab>();
	int32 FoundTabDistance = INT32_MAX;

	for (auto &Tab : FoundTabs)
	{
		TSharedPtr<SDockTab> CurrTabWidget = ActiveTabManager->FindExistingLiveTab(
			Tab.TabId);

		if (!CurrTabWidget.IsValid())
		{
			continue;
		}

		FString CurrName = CurrTabWidget->GetTabLabel().ToString().ToUpper();
		if (CurrName == TargetTabName || CurrName.Contains(TargetTabName))
		{
			FoundTabWidget = CurrTabWidget;
			break;
		}

		int32 Distance = Algo::LevenshteinDistance(CurrName, TargetTabName);
		if (Distance < FoundTabDistance)
		{
			FoundTabWidget = CurrTabWidget;
			FoundTabDistance = Distance;
		}
	}

	if (!FoundTabWidget.IsValid())
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("SelectTabInStack: No Tab Found."))
		return;
	}

	FGlobalTabmanager::Get()->SetActiveTab(FoundTabWidget);
	FoundTabWidget->ActivateInParent(SetDirectly);
}

void UWindowInteractionLibrary::UndoAction(FParseRecord &Record)
{
	GEditor->UndoTransaction();
}

void UWindowInteractionLibrary::RedoAction(FParseRecord &Record)
{
	GEditor->RedoTransaction();
}
