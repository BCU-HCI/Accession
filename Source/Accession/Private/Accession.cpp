// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "Accession.h"
#include "AccessionCommunication.h"
#include "AccessionCommunicationSubsystem.h"
#include "AccessionLogging.h"
#include "AccessionAnalytics.h"

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
#include "FunctionalityWrappers/GraphAddNodeContextMenu.h"
#include "FunctionalityWrappers/GraphLocomotionContext.h"

#include "GraphActionNode.h"
#include "SGraphPanel.h"
#include "FunctionalityWrappers/GraphEditorContext.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Input/SSearchBox.h"

#include "Framework/Docking/TabManager.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Logging/StructuredLog.h"

#define LOCTEXT_NAMESPACE "FAccessionModule"

void FAccessionModule::StartupModule()
{
	UE_LOG(LogAccession, Display, TEXT("AccessionModule::StartupModule()"));

	// Create the Asset Registry
	AssetRegistry = MakeShared<FAccessionAssetRegistry, ESPMode::ThreadSafe>();

	// Register the  Node Factory
	NodeFactory = MakeShared<FAccessionNodeFactory, ESPMode::ThreadSafe>();
	FEdGraphUtilities::RegisterVisualNodeFactory(NodeFactory);

	// Construct Base Phrase Tree Libraries
	FAccessionCommunicationModule::Get()
		.PhraseTreeUtils->RegisterFunctionLibrary(
			NewObject<ULocalizedInputLibrary>());

	FAccessionCommunicationModule::Get()
		.PhraseTreeUtils->RegisterFunctionLibrary(
			NewObject<UWindowInteractionLibrary>());

	FAccessionCommunicationModule::Get()
		.PhraseTreeUtils->RegisterFunctionLibrary(
			NewObject<UViewInteractionLibrary>());

	FAccessionCommunicationModule::Get()
		.PhraseTreeUtils->RegisterFunctionLibrary(
			NewObject<UNodeInteractionLibrary>());

	CreateTranscriptionVisualization();

	FSlateApplication::Get().OnFocusChanging().AddStatic(&FAccessionModule::FocusChangeListener);

	// Register Console Commands
	RegisterConsoleCommands();
}

void FAccessionModule::ShutdownModule()
{
	UE_LOG(LogAccession, Display, TEXT("AccessionModule::ShutdownModule()"));

	UnregisterConsoleCommands();
}

void FAccessionModule::FocusChangeListener(const FFocusEvent &FocusEvent, const FWeakWidgetPath &PrevWidgetPath,
										   const TSharedPtr<SWidget> &PrevFocusedWidget, const FWidgetPath &NewWidgetPath,
										   const TSharedPtr<SWidget> &NewFocusedWidget)
{
	if (!NewFocusedWidget.IsValid())
		return;

	switch (FocusEvent.GetCause())
	{
	case EFocusCause::Mouse:
		OA_LOG(LogAccession, Log, TEXT("WIDGET_FOCUS_MOUSE"), TEXT("Mouse Widget Focus Changed: %s"), *NewFocusedWidget->GetTypeAsString());
		break;

	default:
		break;
	}
}

void FAccessionModule::CreateTranscriptionVisualization()
{
	TranscriptionVisualizer = MakeShared<FTranscriptionVisualizer, ESPMode::ThreadSafe>();

	UAccessionCommunicationSubsystem* ACSubsystem = GEditor->GetEditorSubsystem<UAccessionCommunicationSubsystem>();
	if (ACSubsystem == nullptr)
	{
		UE_LOG(LogAccession, Warning, TEXT("Accession - Transcription Visualiser Cannot Bind Transcript Delegate."))
	}

	// ACSubsystem->OnTranscriptionReceived.AddUniqueDynamic(TranscriptionVisualizer.Get(), &FTranscriptionVisualizer::OnTranscriptionRecieved);
}

void FAccessionModule::RegisterConsoleCommands()
{
	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Accession.Debug.SendPhraseEvent"),
		TEXT("Sends the provided Phrase to the Phrase Tree, replicating the STT Communication Module's Transcription Recieving."),
		FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString> &Args)
													  {
			if (Args.Num() == 0)
				return;

			FString ProvidedPhrase;
			for (const FString& Arg : Args)
			{
				ProvidedPhrase += Arg + TEXT(" ");
			}

			ProvidedPhrase.TrimStartAndEndInline();
			ProvidedPhrase.ToUpperInline();

			UAccessionCommunicationSubsystem* ACSubsystem = GEditor->GetEditorSubsystem<UAccessionCommunicationSubsystem>();
			if (ACSubsystem == nullptr)
				return;

            ACSubsystem->OnTranscriptionReceived.Broadcast(TArray<FString>{ ProvidedPhrase }); }),

		ECVF_Default));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Accession.Debug.LogActiveIndexes"),
		TEXT("Logs the Active Indexes of the Active Tab"),

		FConsoleCommandDelegate::CreateLambda([this]()
											  {

			TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
			SGraphEditor* ActiveGraphEditor = (SGraphEditor*)ActiveTab->GetContent().ToSharedPtr().Get();
			if (ActiveGraphEditor == nullptr)
			{
				UE_LOG(LogAccession, Display, TEXT("Active Tab Not SGraphEditor"));
				return;
			}


			TSharedRef<FGraphIndexer> GraphIndexer = AssetRegistry->GetGraphIndexer(ActiveGraphEditor->GetCurrentGraph()); }),

		ECVF_Default));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Accession.Debug.Graph_AddNodeMenu"),
		TEXT("Opens the context menu for adding nodes for the active graph editor."),

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
					if (!ActiveGraphEditor.IsValid())
					{
						UE_LOG(LogAccession, Display, TEXT("Active Tab Not SGraphEditor"));
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
						UE_LOG(LogAccession, Display, TEXT("Cannot get Keyboard Focused Widget."));
						return;
					}

					UE_LOG(LogAccession, Display, TEXT("Keyboard Focused Widget Type: %s"), *KeyboardFocusedWidget->GetTypeAsString());

					// Getting Menu Object
					FWidgetPath KeyboardFocusedWidgetPath;
					if (FSlateApplication::Get().FindPathToWidget(KeyboardFocusedWidget.ToSharedRef(), KeyboardFocusedWidgetPath))
					{
						UE_LOG(LogAccession, Display, TEXT("Keyboard Focused Widget Path Found."));
					}
					else
						return;

					Menu = FSlateApplication::Get().FindMenuInWidgetPath(KeyboardFocusedWidgetPath);

					// Getting Graph Action Menu Object
					GraphActionMenu = StaticCastSharedPtr<SGraphActionMenu>(
						KeyboardFocusedWidget
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget());

					SearchBox = StaticCastSharedPtr<SSearchBox>(
						KeyboardFocusedWidget
							->GetParentWidget()
							->GetParentWidget()
							->GetParentWidget());

					TSharedRef<SWidget> SearchBoxSibling = SearchBox->GetParentWidget()->GetChildren()->GetChildAt(1);
					TreeView = StaticCastSharedRef<STreeView<TSharedPtr<FGraphActionNode>>>(
						SearchBoxSibling->GetChildren()->GetChildAt(0)->GetChildren()->GetChildAt(0));

					UE_LOG(LogAccession, Log, TEXT("THIS IS THE STRING: %s"), *TreeView->GetTypeAsString());

					MenuWindow = FSlateApplication::Get().FindWidgetWindow(KeyboardFocusedWidget.ToSharedRef());
				}

				UGraphAddNodeContextMenu *AddNodeContextMenu = NewObject<UGraphAddNodeContextMenu>();
				AddNodeContextMenu->AddToRoot();
				AddNodeContextMenu->Init(
					Menu.ToSharedRef(),
					FAccessionCommunicationModule::Get().PhraseTree->AsShared());

				AddNodeContextMenu->ScaleMenu(1.5f);

				FSlateApplication::Get().SetKeyboardFocus(TreeView);

				FPhraseTreeContextManager &ContextManager = FAccessionCommunicationModule::Get()
																.PhraseTree->GetContextManager();

				ContextManager.PushContextObject(AddNodeContextMenu);
			}),

		ECVF_Default));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Accession.Debug.Graph_GenericContextMenu"),
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
						UE_LOG(LogAccession, Display, TEXT("Active Tab Not SGraphEditor"));
						return;
					}
				}

				SGraphPanel *ActiveGraphPanel = ActiveGraphEditor->GetGraphPanel();

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
							DisplayMetrics.PrimaryDisplayHeight / 5);
					}
				}

				TSharedPtr<SWidget> ContextWidgetToFocus = ActiveGraphPanel->SummonContextMenu(
					SpawnLocation,
					ActiveGraphPanel->GetPastePosition(),
					nullptr,
					nullptr,
					TArray<UEdGraphPin *>());

				FWidgetPath ContextWidgetToFocusPath;
				if (FSlateApplication::Get().FindPathToWidget(ContextWidgetToFocus.ToSharedRef(), ContextWidgetToFocusPath))
				{
					UGraphEditorContext *GraphContext = NewObject<UGraphEditorContext>();
					GraphContext->AddToRoot();

					GraphContext->Init(
						FSlateApplication::Get().FindMenuInWidgetPath(ContextWidgetToFocusPath).ToSharedRef(),
						FAccessionCommunicationModule::Get().PhraseTree->AsShared()

					);

					GraphContext->ScaleMenu(1.5f);
				}
			})));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Accession.Debug.DumpActiveTabManagerObject"),
		TEXT("Dumps the Active FTabManager to a JSON Object File"),

		FConsoleCommandDelegate::CreateLambda(
			[this]()
			{
				TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
				if (!ActiveTab.IsValid())
				{
					UE_LOG(LogAccessionPhraseEvent, Display, TEXT("DumpActiveTabManagerObject: No Active Tab Was Found"));
					return;
				}

				TSharedPtr<FTabManager> ActiveTabManager = ActiveTab->GetTabManagerPtr();
				if (!ActiveTabManager.IsValid())
				{
					UE_LOG(LogAccession, Display, TEXT("DumpActiveTabManagerObject: No Parent Tab Manager Found For Active Tab."))
					return;
				}

				TSharedRef<FTabManager::FLayout> ManagerLayout = ActiveTabManager->PersistLayout();

				FString JsonString;
				if (!FJsonSerializer::Serialize(ManagerLayout->ToJson(), TJsonWriterFactory<>::Create(&JsonString, 0)))
				{
					UE_LOG(LogAccession, Display, TEXT("DumpActiveTabManagerObject: Failed to Serialize Json Object to String."))
					return;
				}

				if (!FFileHelper::SaveStringToFile(
						JsonString,
						*FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + TEXT("Accession/Debug/ActiveTabManager_Dump.json"))))
				{
					UE_LOG(LogAccession, Display, TEXT("DumpActiveTabManagerObject: Failed to Save Serialized JSON Object to File."))
				}
			})));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Accession.Debug.Graph_SummonImprovedLocomotion"),
		TEXT("Summons the Improved Locomotion Menu for the Active Graph Editor."),

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
					if (!ActiveGraphEditor.IsValid() || ActiveGraphEditor->GetTypeAsString() != "SGraphEditor")
					{
						UE_LOG(LogAccession, Display, TEXT("Active Tab Not SGraphEditor"));
						return;
					}
				}

				UGraphLocomotionContext *LocomotionContext = NewObject<UGraphLocomotionContext>();
				LocomotionContext->AddToRoot();
				LocomotionContext->Init(ActiveGraphEditor.ToSharedRef());

				FPhraseTreeContextManager &ContextManager = FAccessionCommunicationModule::Get()
																.PhraseTree->GetContextManager();

				ContextManager.PushContextObject(LocomotionContext);
			}),

		ECVF_Default));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Accession.Debug.LogGraphEditorViewport"),
		TEXT("Logs the Active GraphEditors Viewport Information."),
		FConsoleCommandDelegate::CreateLambda(
			[this]()
			{
				TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
				if (!ActiveTab.IsValid())
				{
					UE_LOG(LogAccession, Display, TEXT("No Active Tab Found."));
					return;
				}

				TSharedPtr<SGraphEditor> ActiveGraphEditor = StaticCastSharedRef<SGraphEditor>(ActiveTab->GetContent());
				if (!ActiveGraphEditor.IsValid())
				{
					UE_LOG(LogAccession, Display, TEXT("Active Tab Not SGraphEditor"));
					return;
				}

				FVector2D ViewLocation;
				float ZoomAmount;
				ActiveGraphEditor->GetViewLocation(ViewLocation, ZoomAmount);

				UE_LOG(LogAccession, Display, TEXT("| Active Graph Editor Viewport Info | View Location: %s | Zoom Amount: %f |"), *ViewLocation.ToString(), ZoomAmount);
			})));
}

void FAccessionModule::UnregisterConsoleCommands()
{
	IConsoleCommand *ConsoleCommand = nullptr;
	while (ConsoleCommands.Num() > 0)
	{
		ConsoleCommand = ConsoleCommands.Pop();

		IConsoleManager::Get().UnregisterConsoleObject(ConsoleCommand);

		delete ConsoleCommand;
		ConsoleCommand = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAccessionModule, Accession)