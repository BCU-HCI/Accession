// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAccessibility.h"
#include "OpenAccessibilityCommunication.h"
#include "OpenAccessibilityLogging.h"

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/PhraseInputNode.h"
#include "PhraseTree/PhraseStringInputNode.h"
#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/PhraseContextNode.h"
#include "PhraseTree/PhraseContextMenuNode.h"
#include "PhraseTree/PhraseEventNode.h"

#include "PhraseEvents/LocalizedInputLibrary.h"
#include "PhraseEvents/WindowInteractionLibrary.h"
#include "PhraseEvents/ViewInteractionLibrary.h"
#include "PhraseEvents/NodeInteractionLibrary.h"

#include "TranscriptionVisualizer.h"
#include "AccessibilityWrappers/AccessibilityAddNodeContextMenu.h"
#include "AccessibilityWrappers/AccessibilityGraphLocomotionContext.h"

#include "GraphActionNode.h"
#include "SGraphPanel.h"
#include "AccessibilityWrappers/AccessibilityGraphEditorContext.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Input/SSearchBox.h"

#include "Framework/Docking/TabManager.h"
#include "Logging/StructuredLog.h"

#define LOCTEXT_NAMESPACE "FOpenAccessibilityModule"

void FOpenAccessibilityModule::StartupModule()
{
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::StartupModule()"));

	// Create the Asset Registry
	AssetAccessibilityRegistry = MakeShared<FAssetAccessibilityRegistry, ESPMode::ThreadSafe>();

	// Register the Accessibility Node Factory
	AccessibilityNodeFactory = MakeShared<FAccessibilityNodeFactory, ESPMode::ThreadSafe>();
	FEdGraphUtilities::RegisterVisualNodeFactory(AccessibilityNodeFactory);

	// Construct Base Phrase Tree Libraries
	FOpenAccessibilityCommunicationModule::Get()
	.PhraseTreeUtils->RegisterFunctionLibrary(
		NewObject<ULocalizedInputLibrary>()
	);

	FOpenAccessibilityCommunicationModule::Get()
	.PhraseTreeUtils->RegisterFunctionLibrary(
		NewObject<UWindowInteractionLibrary>()
	);

	FOpenAccessibilityCommunicationModule::Get()
	.PhraseTreeUtils->RegisterFunctionLibrary(
		NewObject<UViewInteractionLibrary>()
	);

	FOpenAccessibilityCommunicationModule::Get()
	.PhraseTreeUtils->RegisterFunctionLibrary(
		NewObject<UNodeInteractionLibrary>()
	);

	CreateTranscriptionVisualization();

	// Register Console Commands
	RegisterConsoleCommands();
}

void FOpenAccessibilityModule::ShutdownModule()
{
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::ShutdownModule()"));

	UnregisterConsoleCommands();
}

void FOpenAccessibilityModule::CreateTranscriptionVisualization()
{
	TranscriptionVisualizer = MakeShared<FTranscriptionVisualizer, ESPMode::ThreadSafe>();

	FOpenAccessibilityCommunicationModule::Get().OnTranscriptionRecieved
		.AddSP(TranscriptionVisualizer.ToSharedRef(), &FTranscriptionVisualizer::OnTranscriptionRecieved);
}

void FOpenAccessibilityModule::RegisterConsoleCommands()
{
	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibility.Debug.SendPhraseEvent"), 
		TEXT("Sends the provided Phrase to the Phrase Tree, replicating the STT Communication Module's Transcription Recieving."),
        FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString> &Args) {
			if (Args.Num() == 0)
				return;

			FString ProvidedPhrase;
			for (const FString& Arg : Args)
			{
				ProvidedPhrase += Arg + TEXT(" ");
			}

			ProvidedPhrase.TrimStartAndEndInline();
			ProvidedPhrase.ToUpperInline();

            FOpenAccessibilityCommunicationModule::Get()
				.OnTranscriptionRecieved.Broadcast(TArray<FString>{ ProvidedPhrase });
        }),

		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibility.Debug.LogActiveIndexes"),
		TEXT("Logs the Active Indexes of the Active Tab"),

		FConsoleCommandDelegate::CreateLambda([this]() {

			TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
			SGraphEditor* ActiveGraphEditor = (SGraphEditor*)ActiveTab->GetContent().ToSharedPtr().Get();
			if (ActiveGraphEditor == nullptr)
			{
				UE_LOG(LogOpenAccessibility, Display, TEXT("Active Tab Not SGraphEditor"));
				return;
			}


			TSharedRef<FGraphIndexer> GraphIndexer = AssetAccessibilityRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph());
		}),

		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibility.Debug.OpenAccessibilityGraph_AddNodeMenu"),
		TEXT("Opens the context menu for adding nodes for the active graph editor."),

		FConsoleCommandDelegate::CreateLambda(
			[this]() {

				TSharedPtr<SGraphEditor> ActiveGraphEditor = nullptr;
				{
					// Getting Graph Editor Section

					TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
					if (!ActiveTab.IsValid())
						return;

					ActiveGraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab->GetContent().ToSharedPtr());
					if (!ActiveGraphEditor.IsValid())
					{
						UE_LOG(LogOpenAccessibility, Display, TEXT("Active Tab Not SGraphEditor"));
						return;
					}
				}

				TSharedPtr<IMenu> Menu;
				TSharedPtr<SWindow> MenuWindow;
				TSharedPtr<SGraphActionMenu> GraphActionMenu;
				TSharedPtr<SSearchBox> SearchBox;
				TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeView;
				{
					// Summoning Create Node Menu Section
					// and Getting any Key Widgets

					ActiveGraphEditor->GetGraphPanel()->SummonCreateNodeMenuFromUICommand(0);

					TSharedPtr<SWidget> KeyboardFocusedWidget = StaticCastSharedPtr<SEditableText>(FSlateApplication::Get().GetKeyboardFocusedWidget());
					if (!KeyboardFocusedWidget.IsValid())
					{
						UE_LOG(LogOpenAccessibility, Display, TEXT("Cannot get Keyboard Focused Widget."));
						return;
					}

					UE_LOG(LogOpenAccessibility, Display, TEXT("Keyboard Focused Widget Type: %s"), *KeyboardFocusedWidget->GetTypeAsString());

					// Getting Menu Object
					FWidgetPath KeyboardFocusedWidgetPath;
					if (FSlateApplication::Get().FindPathToWidget(KeyboardFocusedWidget.ToSharedRef(), KeyboardFocusedWidgetPath))
					{
						UE_LOG(LogOpenAccessibility, Display, TEXT("Keyboard Focused Widget Path Found."));
					}
					else return;

					Menu = FSlateApplication::Get().FindMenuInWidgetPath(KeyboardFocusedWidgetPath);

					// Getting Graph Action Menu Object
					GraphActionMenu = StaticCastSharedPtr<SGraphActionMenu>(
						KeyboardFocusedWidget
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget()
					);

					SearchBox = StaticCastSharedPtr<SSearchBox>(
						KeyboardFocusedWidget
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget()
					);

					TSharedRef<SWidget> SearchBoxSibling = SearchBox->GetParentWidget()->GetChildren()->GetChildAt(1);
					TreeView = StaticCastSharedRef<STreeView<TSharedPtr<FGraphActionNode>>>(
						SearchBoxSibling->GetChildren()->GetChildAt(0)->GetChildren()->GetChildAt(0)
					);

					UE_LOG(LogOpenAccessibility, Log, TEXT("THIS IS THE STRING: %s"), *TreeView->GetTypeAsString());

					MenuWindow = FSlateApplication::Get().FindWidgetWindow(KeyboardFocusedWidget.ToSharedRef());
				}
				
				UAccessibilityAddNodeContextMenu* AddNodeContextMenu = NewObject<UAccessibilityAddNodeContextMenu>();
				AddNodeContextMenu->AddToRoot();
				AddNodeContextMenu->Init(
					Menu.ToSharedRef(),
					FOpenAccessibilityCommunicationModule::Get().PhraseTree->AsShared()
				);

				AddNodeContextMenu->ScaleMenu(1.5f);

				FSlateApplication::Get().SetKeyboardFocus(TreeView);

				FPhraseTreeContextManager& ContextManager =FOpenAccessibilityCommunicationModule::Get()
                      .PhraseTree->GetContextManager();

				ContextManager.PushContextObject(AddNodeContextMenu);
			}),

		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibility.Debug.OpenAccessibilityGraph_GenericContextMenu"),
		TEXT("Opens the Context Menu for the Active Graph Editor, and Uses Generic Bindings For Commands"),

		FConsoleCommandDelegate::CreateLambda(
			[this]()
			{
				TSharedPtr<SGraphEditor> ActiveGraphEditor = nullptr;
				{
					// Getting Graph Editor Section

					TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
					if (!ActiveTab.IsValid())
						return;

					ActiveGraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab->GetContent().ToSharedPtr());
					if (!ActiveGraphEditor.IsValid() && ActiveGraphEditor->GetType() == "SGraphEditor")
					{
						UE_LOG(LogOpenAccessibility, Display, TEXT("Active Tab Not SGraphEditor"));
						return;
					}
				}

				SGraphPanel* ActiveGraphPanel = ActiveGraphEditor->GetGraphPanel();

				FVector2D SpawnLocation;
				{
					TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelRegularWindow();
					if (TopLevelWindow.IsValid())
					{
						SpawnLocation = TopLevelWindow->GetPositionInScreen();
						FVector2D WindowSize = TopLevelWindow->GetSizeInScreen();

						SpawnLocation.X += WindowSize.X / 5;
						SpawnLocation.Y += WindowSize.Y / 5;
					}
					else
					{
						FDisplayMetrics DisplayMetrics;
						FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

						SpawnLocation = FVector2D(
							DisplayMetrics.PrimaryDisplayWidth / 5,
							DisplayMetrics.PrimaryDisplayHeight / 5
						);
					}					
				}

				TSharedPtr<SWidget> ContextWidgetToFocus = ActiveGraphPanel->SummonContextMenu(
					SpawnLocation,
					ActiveGraphPanel->GetPastePosition(),
					nullptr,
					nullptr,
					TArray<UEdGraphPin*>()
				);

				FWidgetPath ContextWidgetToFocusPath;
				if (FSlateApplication::Get().FindPathToWidget(ContextWidgetToFocus.ToSharedRef(), ContextWidgetToFocusPath))
				{
					UAccessibilityGraphEditorContext* GraphContext = NewObject<UAccessibilityGraphEditorContext>();
					GraphContext->AddToRoot();

					GraphContext->Init(
						FSlateApplication::Get().FindMenuInWidgetPath(ContextWidgetToFocusPath).ToSharedRef(),
						FOpenAccessibilityCommunicationModule::Get().PhraseTree->AsShared()

					);

					GraphContext->ScaleMenu(1.5f); 
				}

			}
		)
	)),

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibility.Debug.OpenAccessibilityGraph_SummonImprovedLocomotion"),
		TEXT("Summons the Improved Locomotion Menu for the Active Graph Editor."),

		FConsoleCommandDelegate::CreateLambda(
			[this]() {
				TSharedPtr<SGraphEditor> ActiveGraphEditor = nullptr;
				{
					// Getting Graph Editor Section

					TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
					if (!ActiveTab.IsValid())
						return;

					ActiveGraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab->GetContent().ToSharedPtr());
					if (!ActiveGraphEditor.IsValid() || ActiveGraphEditor->GetTypeAsString() != "SGraphEditor") 
					{
						UE_LOG(LogOpenAccessibility, Display, TEXT("Active Tab Not SGraphEditor"));
						return;
					}
				}

				UAccessibilityGraphLocomotionContext* LocomotionContext = NewObject<UAccessibilityGraphLocomotionContext>();
				LocomotionContext->AddToRoot();
				LocomotionContext->Init(ActiveGraphEditor.ToSharedRef());

				FPhraseTreeContextManager& ContextManager = FOpenAccessibilityCommunicationModule::Get()
                  .PhraseTree->GetContextManager();

				ContextManager.PushContextObject(LocomotionContext);
			}),

		ECVF_Default
	));
}

void FOpenAccessibilityModule::UnregisterConsoleCommands()
{
	IConsoleCommand* ConsoleCommand = nullptr;
	while (ConsoleCommands.Num() > 0)
	{
		ConsoleCommand = ConsoleCommands.Pop();

		IConsoleManager::Get().UnregisterConsoleObject(ConsoleCommand);

		delete ConsoleCommand;
		ConsoleCommand = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenAccessibilityModule, OpenAccessibility)