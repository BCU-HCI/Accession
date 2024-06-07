// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWrappers/AccessibilityWindowToolbar.h"
#include "AccessibilityWidgets/SContentIndexer.h"

#include "PhraseTree/Containers/ParseRecord.h"
#include "PhraseTree/Containers/Input/UParseIntInput.h"

UAccessibilityWindowToolbar::UAccessibilityWindowToolbar() : UObject()
{
	ToolbarIndex = MakeUnique<FIndexer<int32, SMultiBlockBaseWidget*>>();

	LastToolkit = TWeakPtr<SWidget>();
	LastTopWindow = TWeakPtr<SWindow>();
	LastToolkitParent = TWeakPtr<SBorder>();

	BindTicker();
}

UAccessibilityWindowToolbar::~UAccessibilityWindowToolbar()
{
	UnbindTicker();
}

bool UAccessibilityWindowToolbar::Tick(float DeltaTime)
{
	TSharedPtr<SWindow> TopWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();
	if (!TopWindow.IsValid())
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("AccessibilityToolBar: Window Is Not Valid."));
		return false;
	}

	TSharedPtr<SBorder> ContentContainer;
	if (TopWindow != LastTopWindow)
		ContentContainer = GetWindowContentContainer(TopWindow.ToSharedRef());
	else ContentContainer = LastToolkitParent.Pin();

	TSharedPtr<SWidget> Toolkit = ContentContainer->GetContent();
	if (!Toolkit.IsValid())
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("AccessibilityToolBar: Toolkit Is Not Valid."));
		return false;
	}

	/*
	if (!IndexedToolkits.Contains(Toolkit.Get()))
	{
		ApplyToolbarIndexing(Toolkit.ToSharedRef());
	}
	*/

	ApplyToolbarIndexing(Toolkit.ToSharedRef(), TopWindow.ToSharedRef());

	LastToolkit = Toolkit;
	LastTopWindow = TopWindow;
	LastToolkitParent = ContentContainer;

	return true;
}

void UAccessibilityWindowToolbar::ApplyToolbarIndexing(TSharedRef<SWidget> ToolkitWidget, TSharedRef<SWindow> ToolkitWindow)
{
	TSharedPtr<SWidget> ToolBarContainer;
	if (!GetToolKitToolBar(ToolkitWidget, ToolBarContainer))
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("Failed to get Toolbar."));
		return;
	}

	ToolbarIndex->Empty();

	if (!ToolBarContainer.IsValid())
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("Toolbar Container Is Not Valid."));
		return;
	}

	TArray<FChildren*> ChildrenToFilter = TArray<FChildren*> {
		ToolBarContainer->GetChildren()
	};

	FString WidgetType;
	TSet<FString> AllowedWidgetTypes = TSet<FString>{
		TEXT("SToolBarButtonBlock"),
		TEXT("SToolBarComboButtonBlock"),
		TEXT("SToolBarStackButtonBlock"),
		TEXT("SUniformToolBarButtonBlock")
	};

	int32 Index = -1;
	while (ChildrenToFilter.Num() > 0)
	{
		FChildren* Children = ChildrenToFilter[0];
		ChildrenToFilter.RemoveAt(0);

		// To-Do: Learn How to Write Readable Code.
		for (int i = 0; i < Children->NumSlot(); i++)
		{
			FSlotBase& ChildSlot = const_cast<FSlotBase&>(Children->GetSlotAt(i));

			TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(i);
			if (!ChildWidget.IsValid())
				continue;

			if (ChildWidget->GetDesiredSize() == FVector2D::ZeroVector)
				continue;
			
			WidgetType = ChildWidget->GetTypeAsString();
			
			if (ChildWidget.IsValid() && AllowedWidgetTypes.Contains(WidgetType))
			{
				TSharedPtr<SMultiBlockBaseWidget> ToolBarButtonWidget = StaticCastSharedPtr<SMultiBlockBaseWidget>(ChildWidget);

				ChildSlot.DetachWidget();

				ToolbarIndex->GetKeyOrAddValue(
					ToolBarButtonWidget.Get(), 
					Index
				);

				ChildSlot.AttachWidget(
					SNew(SContentIndexer)
					.IndexValue(Index)
					.IndexPositionToContent(EIndexerPosition::Bottom)
					.ContentToIndex(ToolBarButtonWidget)
                    .Visibility_Lambda([ToolkitWindow]() -> EVisibility {
						if (FSlateApplication::Get().GetActiveTopLevelRegularWindow() == ToolkitWindow)
							return EVisibility::Visible;
                        else return EVisibility::Hidden;
					})

				);
			} 
			else if (ChildWidget.IsValid() && WidgetType == "SContentIndexer")
			{
				TSharedPtr<SContentIndexer> IndexerWidget = StaticCastSharedPtr<SContentIndexer>(ChildWidget);

				TSharedPtr<SMultiBlockBaseWidget> IndexedContent = StaticCastSharedPtr<SMultiBlockBaseWidget>(IndexerWidget->GetContent());
				if (!IndexedContent.IsValid())
					continue;

				ToolbarIndex->GetKeyOrAddValue(
					IndexedContent.Get(),
					Index
				);

				IndexerWidget->UpdateIndex(Index);
			}
			else ChildrenToFilter.Add(ChildWidget->GetChildren());
		}
	}

	IndexedToolkits.Add(ToolkitWidget.ToSharedPtr().Get());
}

// -- Util Widget Function -- 

template<typename T = SWidget>
FORCEINLINE TSharedPtr<T> GetWidgetDescendantOfType(TSharedRef<SWidget> Widget, FName TypeName)
{
	if (Widget->GetType() == TypeName)
	{
		return Widget;
	}

	TArray<FChildren*> ChildrenToFilter;
	ChildrenToFilter.Add(Widget->GetChildren());

	while (ChildrenToFilter.Num() > 0)
	{
		FChildren* Children = ChildrenToFilter.Pop();

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

void UAccessibilityWindowToolbar::SelectToolbarItem(FParseRecord& Record)
{
	UParseIntInput* Input = Record.GetPhraseInput<UParseIntInput>("INDEX");
	if (!Input->IsValidLowLevelFast())
		return;

	SMultiBlockBaseWidget* LinkedButton = ToolbarIndex->GetValue(Input->GetValue());
	if (LinkedButton == nullptr)
		return;

	// Might Work?
	// Just Might break in future.

	// LinkedButton->SimulateClick();
}

TSharedPtr<SBorder> UAccessibilityWindowToolbar::GetWindowContentContainer(TSharedRef<SWindow> WindowToFindContainer)
{
	// Find SDockingTabStack
	TSharedPtr<SWidget> DockingTabStack = GetWidgetDescendantOfType(WindowToFindContainer, "SDockingTabStack");
	if (!DockingTabStack.IsValid())
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("DockingTabStack is not Valid"));
		return nullptr;
	}

	return StaticCastSharedRef<SBorder>(
		DockingTabStack
			->GetChildren()->GetChildAt(0) // SVerticalBox
			->GetChildren()->GetChildAt(1) // SOverlay
			->GetChildren()->GetChildAt(0) // SBorder
	);
}

bool UAccessibilityWindowToolbar::GetToolKitToolBar(TSharedRef<SWidget> ToolKitWidget, TSharedPtr<SWidget>& OutToolBar)
{
	TSharedPtr<SWidget> CurrChild;
	FChildren* CurrChildren = ToolKitWidget->GetChildren();
	if (CurrChildren->Num() == 0) 
		return false;

	CurrChild = CurrChildren->GetChildAt(0); // Get SVerticalBox
	CurrChildren = CurrChild->GetChildren();
	if (CurrChildren->Num() == 0)
		return false;

	CurrChild = CurrChildren->GetChildAt(0); // Get SOverlay
	CurrChildren = CurrChild->GetChildren();
	if (CurrChildren->Num() < 2)
		return false;

	OutToolBar = CurrChildren->GetChildAt(1); // Get SHorizontalBox
        if (OutToolBar.IsValid()) return true;
        else return false;
}

void UAccessibilityWindowToolbar::BindTicker()
{
	FTickerDelegate TickDelegate = FTickerDelegate::CreateUObject(this, &UAccessibilityWindowToolbar::Tick);

	TickDelegateHandle = FTSTicker::GetCoreTicker()
		.AddTicker(TickDelegate);
}

void UAccessibilityWindowToolbar::UnbindTicker()
{
	FTSTicker::GetCoreTicker()
		.RemoveTicker(TickDelegateHandle);
}
