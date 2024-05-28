// Copyright F-Dudley. All Rights Reserved.

#include "AccessibilityWrappers/AccessibilityWindowToolbar.h"
#include "AccessibilityWidgets/SContentIndexer.h"

#include "PhraseTree/Containers/Input/UParseIntInput.h"

UAccessibilityWindowToolbar::UAccessibilityWindowToolbar() : UObject()
{
	ToolbarIndex = MakeUnique<FIndexer<int32, SMultiBlockBaseWidget*>>();

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
		return false;

	TSharedPtr<SBorder> ContentContainer = GetWindowContentContainer(TopWindow.ToSharedRef());
	TSharedPtr<SWidget> Toolkit = ContentContainer->GetContent();
	if (!ContentContainer.IsValid())
		return false;

	if (Toolkit == LastToolkit && TopWindow == LastTopWindow)
		return false;

	ApplyToolbarIndexing(ContentContainer->GetContent());

	LastToolkit = Toolkit;
	LastTopWindow = TopWindow;
	LastToolkitParent = ContentContainer;

	return true;
}

void UAccessibilityWindowToolbar::ApplyToolbarIndexing(TSharedRef<SWidget> ToolkitWidget)
{
	ToolbarIndex->Empty();

	TSharedPtr<SWidget> ToolbarContainer = ToolkitWidget
		->GetChildren()->GetChildAt(0)  // Get SVerticalBox
		->GetChildren()->GetChildAt(0)  // Get SOverlay
		->GetChildren()->GetChildAt(1); // Get SHorizontalBox

	if (!ToolbarContainer.IsValid())
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("Toolbar Container is not valid."));
		return;
	}

	TArray<FChildren*> ChildrenToFilter;
	ChildrenToFilter.Add(ToolbarContainer->GetChildren());

	TSharedPtr<SWidget> ChildWidget;
	FString WidgetType;

	int32 Index = -1;
	while (ChildrenToFilter.Num() > 0)
	{
		FChildren* Children = ChildrenToFilter[0];
		ChildrenToFilter.RemoveAt(0);

		// To-Do: Learn How to Write Readable Code.
		for (int i = 0; i < Children->NumSlot(); i++)
		{
			ChildWidget = Children->GetChildAt(i);
			WidgetType = ChildWidget->GetTypeAsString();

			FSlotBase& ChildSlot = const_cast<FSlotBase&>(Children->GetSlotAt(i));

			TSharedPtr<SContentIndexer> ContentIndexer = SNew(SContentIndexer)
				.IndexValue(Index)
				.IndexPositionToContent(EIndexerPosition::Bottom)
				.ContentToIndex(ChildWidget);

			ChildSlot.AttachWidget(ContentIndexer.ToSharedRef());

			ChildrenToFilter.Add(ChildWidget->GetChildren());
		}
	}
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

	SMultiBlockBaseWidget* MultiBlockWidget = ToolbarIndex->GetValue(Input->GetValue());
	if (MultiBlockWidget == nullptr)
		return;

	// Add OnClick Functionality Here, Using the MultiBlockWidget.
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
