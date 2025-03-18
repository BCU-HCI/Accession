#include "OpenAccessibilityAnalytics.h"
#include "OpenAccessibilityAnalyticsLogging.h"

#include "HAL/PlatformFileManager.h"
#include "Misc/DateTime.h"

#define LOCTEXT_NAMESPACE "FOpenAccessibilityAnalyticsModule"

void FOpenAccessibilityAnalyticsModule::StartupModule() 
{
	SessionLogFile = GenerateFileForSessionLog();

	AddConsoleCommands();
} 

void FOpenAccessibilityAnalyticsModule::ShutdownModule() 
{
	RemoveConsoleCommands();
}

FString FOpenAccessibilityAnalyticsModule::GenerateFileForSessionLog()
{
	FDateTime CurrentDateTime = FDateTime::Now();

	FString CombinedFileName = TEXT("OAEventLog-") + CurrentDateTime.ToString() + TEXT(".log");
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + TEXT("OpenAccessibility/Logs/") + CombinedFileName);
}

void FOpenAccessibilityAnalyticsModule::AddConsoleCommands()
{
	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibilityAnalytics.Debug.Add_Mock_Event"),
		TEXT("Adds a MOCK Event to the Eventbuffer"),

		FConsoleCommandWithArgsDelegate::CreateLambda(
			[&](const TArray<FString>& Args) {

				if (Args.Num() < 2)
					return;

				FString EventTitle = Args[0];
				FString EventBody;

				for (int i = 1; i < Args.Num(); i++)
				{
					EventBody += Args[i] + TEXT(" ");
				}
				

			}
		)
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibilityAnalytics.Util.GenerateNewLogFile"),
		TEXT("Generates a new log file location, with logging continuing from that location."),

		FConsoleCommandDelegate::CreateLambda(
			[&]() {
				SessionLogFile = GenerateFileForSessionLog();
			}
		)
	));
}

void FOpenAccessibilityAnalyticsModule::RemoveConsoleCommands()
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

IMPLEMENT_MODULE(FOpenAccessibilityAnalyticsModule, OpenAccessibilityAnalytics)