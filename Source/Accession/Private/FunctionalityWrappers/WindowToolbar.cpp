// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "FunctionalityWrappers/WindowToolbarIndex.h"
#include "Widgets/SContentIndexer.h"

#include "PhraseTree/Containers/ParseRecord.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"

UWindowToolbarIndex::UWindowToolbarIndex() : UObject()
{
	LastToolkit = TWeakPtr<SWidget>();
	LastTopWindow = TWeakPtr<SWindow>();
	LastToolkitParent = TWeakPtr<SBorder>();

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibiliy.ToolBar.ShowIndexerStats"),
		TEXT("Displays the Indexer Stats for the Toolbar."),

		FConsoleCommandDelegate::CreateLambda([this]()
											  { UE_LOG(LogAccession, Display, TEXT("| ToolBar Indexer Stats | Indexed Amount: %d | "), ToolbarIndex.Num()) })));

	BindTicker();
}

UWindowToolbarIndex::~UWindowToolbarIndex()
{
	UE_LOG(LogAccession, Log, TEXT("ToolBar: Destroyed."));

	UnbindTicker();
}

bool UWindowToolbarIndex::Tick(float DeltaTime)
{
	TSharedPtr<SWindow> TopWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();
	if (!TopWindow.IsValid())
	{
		return true;
	}

	TSharedPtr<SBorder> ContentContainer;
	if (TopWindow != LastTopWindow)
		ContentContainer = GetWindowContentContainer(TopWindow.ToSharedRef());
	else
		ContentContainer = LastToolkitParent.Pin();

	if (!ContentContainer.IsValid())
	{
		return true;
	}

	TSharedPtr<SWidget> Toolkit = ContentContainer->GetContent();
	if (!Toolkit.IsValid())
	{
		return true;
	}

	if (ApplyToolbarIndexing(Toolkit.ToSharedRef(), TopWindow.ToSharedRef()))
	{
		LastToolkit = Toolkit;
		// UE_LOG(LogAccession, Log, TEXT("AccessionToolBar: Toolkit Indexing Applied To %s"), *Toolkit->GetTypeAsString());
	}

	LastTopWindow = TopWindow;
	LastToolkitParent = ContentContainer;

	return true;
}

bool UWindowToolbarIndex::ApplyToolbarIndexing(TSharedRef<SWidget> ToolkitWidget, TSharedRef<SWindow> ToolkitWindow)
{
	TSharedPtr<SWidget> ToolBarContainer;
	if (!GetToolKitToolBar(ToolkitWidget, ToolBarContainer))
	{
		UE_LOG(LogAccession, Log, TEXT("Failed to get Toolbar."));
		return false;
	}

	if (!ToolBarContainer.IsValid())
	{
		UE_LOG(LogAccession, Log, TEXT("Toolbar Container Is Not Valid."));
		return false;
	}

	TArray<FChildren *> ChildrenToFilter = TArray<FChildren *>{
		ToolBarContainer->GetChildren()};

	FString WidgetType;
	TSet<FString> AllowedWidgetTypes = TSet<FString>{
		TEXT("SToolBarButtonBlock"),
		TEXT("SToolBarComboButtonBlock"),
		TEXT("SToolBarStackButtonBlock"),
		TEXT("SUniformToolBarButtonBlock")};

	ToolbarIndex.Reset();

	int32 Index = -1;
	while (ChildrenToFilter.Num() > 0)
	{
		FChildren *Children = ChildrenToFilter[0];
		ChildrenToFilter.RemoveAt(0);

		// To-Do: Learn How to Write Readable Code.
		for (int i = 0; i < Children->NumSlot(); i++)
		{
			FSlotBase &ChildSlot = const_cast<FSlotBase &>(Children->GetSlotAt(i));

			TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(i);
			if (!ChildWidget.IsValid() || ChildWidget->GetDesiredSize() == FVector2D::ZeroVector)
				continue;

			WidgetType = ChildWidget->GetTypeAsString();

			if (ChildWidget.IsValid() && AllowedWidgetTypes.Contains(WidgetType))
			{
				TSharedPtr<SMultiBlockBaseWidget> ToolBarButtonWidget = StaticCastSharedPtr<SMultiBlockBaseWidget>(ChildWidget);

				ChildSlot.DetachWidget();

				ToolbarIndex.GetKeyOrAddValue(
					ToolBarButtonWidget.Get(),
					Index);

				ChildSlot.AttachWidget(
					SNew(SContentIndexer)
						.IndexValue(Index)
						.IndexPositionToContent(EIndexerPosition::Bottom)
						.ContentToIndex(ToolBarButtonWidget)
						.IndexVisibility_Lambda([this, ToolkitWidget]() -> EVisibility
												{ return (this->IsActiveToolbar(ToolkitWidget))
															 ? EVisibility::Visible
															 : EVisibility::Hidden; }));
			}
			else if (ChildWidget.IsValid() && WidgetType == "SContentIndexer")
			{
				TSharedPtr<SContentIndexer> IndexerWidget = StaticCastSharedPtr<SContentIndexer>(ChildWidget);

				TSharedPtr<SMultiBlockBaseWidget> IndexedContent = StaticCastSharedRef<SMultiBlockBaseWidget>(IndexerWidget->GetContent());
				if (!IndexedContent.IsValid())
					continue;

				ToolbarIndex.GetKeyOrAddValue(
					IndexedContent.Get(),
					Index);

				IndexerWidget->UpdateIndex(Index);
			}
			else
				ChildrenToFilter.Add(ChildWidget->GetChildren());
		}
	}

	return true;
}

// -- Util Widget Function --

template <typename T = SWidget>
FORCEINLINE TSharedPtr<T> GetWidgetDescendantOfType(TSharedRef<SWidget> Widget, FName TypeName)
{
	if (Widget->GetType() == TypeName)
	{
		return Widget;
	}

	TArray<FChildren *> ChildrenToFilter;
	ChildrenToFilter.Add(Widget->GetChildren());

	while (ChildrenToFilter.Num() > 0)
	{
		FChildren *Children = ChildrenToFilter.Pop();

		for (int i = 0; i < Children->Num(); i++)
		{
			TSharedRef<SWidget> Child = Children->GetChildAt(i);

			ChildrenToFilter.Add(Child->GetChildren());

			if (Child->GetType() == TypeName)
			{
				return StaticCastSharedPtr<T>(Child.ToSharedPtr());
			}
		}
	}

	return nullptr;
}

// --  --

void UWindowToolbarIndex::SelectToolbarItem(int32 Index)
{
	if (ToolbarIndex.IsEmpty())
	{
		UE_LOG(LogAccession, Warning, TEXT("ToolBar Index is Empty."))
		return;
	}

	SMultiBlockBaseWidget *LinkedButton;
	if (!ToolbarIndex.GetValue(Index, LinkedButton))
	{
		UE_LOG(LogAccession, Warning, TEXT("Provided Index is Not Linked to a ToolBar Button."))
		return;
	}

	TSharedPtr<const FMultiBlock> MultiBlock = LinkedButton->GetBlock();
	if (!MultiBlock.IsValid())
	{
		UE_LOG(LogAccession, Warning, TEXT("Provided ToolBar MultiBlock is Not Valid."))
	}

	TSharedPtr<const FUICommandList> ActionList = MultiBlock->GetActionList();
	TSharedPtr<const FUICommandInfo> Action = MultiBlock->GetAction();

	if (ActionList.IsValid() && Action.IsValid())
	{
		ActionList->ExecuteAction(Action.ToSharedRef());
	}
	else
	{
		const FUIAction &DirectAction = MultiBlock->GetDirectActions();

		DirectAction.Execute();
	}
}

bool UWindowToolbarIndex::IsActiveToolbar(const TSharedRef<SWidget> &ToolkitWidget)
{
	return LastToolkit.IsValid()
			   ? LastToolkit.Pin() == ToolkitWidget
			   : false;
}

TSharedPtr<SWidget> UWindowToolbarIndex::GetActiveToolkitWidget() const
{
	if (LastToolkit.IsValid())
		return LastToolkit.Pin();

	return TSharedPtr<SWidget>();
}

TSharedPtr<SBorder> UWindowToolbarIndex::GetWindowContentContainer(TSharedRef<SWindow> WindowToFindContainer)
{
	// Find SDockingTabStack
	TSharedPtr<SWidget> DockingTabStack = GetWidgetDescendantOfType(WindowToFindContainer, "SDockingTabStack");
	if (!DockingTabStack.IsValid())
	{
		UE_LOG(LogAccession, Log, TEXT("DockingTabStack is not Valid"));
		return nullptr;
	}

	return StaticCastSharedRef<SBorder>(
		DockingTabStack
			->GetChildren()
			->GetChildAt(0) // SVerticalBox
			->GetChildren()
			->GetChildAt(1) // SOverlay
			->GetChildren()
			->GetChildAt(0) // SBorder
	);
}

bool UWindowToolbarIndex::GetToolKitToolBar(TSharedRef<SWidget> ToolKitWidget, TSharedPtr<SWidget> &OutToolBar)
{
	TSharedPtr<SWidget> CurrChild;
	FChildren *CurrChildren = ToolKitWidget->GetChildren();
	if (CurrChildren->Num() == 0)
		return false;

	CurrChild = CurrChildren->GetChildAt(0); // Get SVerticalBox
	CurrChildren = CurrChild->GetChildren();
	if (CurrChildren->Num() == 0)
		return false;

	OutToolBar = CurrChildren->GetChildAt(0); // Get SHorizontalBox
	if (!OutToolBar.IsValid())
		return false;

	return true;
}

void UWindowToolbarIndex::BindTicker()
{
	FTickerDelegate TickDelegate = FTickerDelegate::CreateUObject(this, &UWindowToolbarIndex::Tick);

	TickDelegateHandle = FTSTicker::GetCoreTicker()
							 .AddTicker(TickDelegate);
}

void UWindowToolbarIndex::UnbindTicker()
{
	FTSTicker::GetCoreTicker()
		.RemoveTicker(TickDelegateHandle);
}
