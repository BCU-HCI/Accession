#include "OpenAccessibilityAnalytics.h"
#include "OpenAccessibilityAnalyticsLogging.h"

#include "HAL/PlatformFileManager.h"
#include "Misc/DateTime.h"

#define LOCTEXT_NAMESPACE "FOpenAccessibilityAnalyticsModule"

void FOpenAccessibilityAnalyticsModule::StartupModule() 
{
	if (SessionBufferFile.IsEmpty())
		SessionBufferFile = GenerateFileForSessionLog();

	EnableDumpTick();
	AddConsoleCommands();
} 

void FOpenAccessibilityAnalyticsModule::ShutdownModule() 
{
	DisableDumpTick();
	RemoveConsoleCommands();
}

bool FOpenAccessibilityAnalyticsModule::DumpTick(float DeltaTime)
{
	if (EventBuffer.IsEmpty())
		return true;

	if (SessionBufferFile.IsEmpty())
		SessionBufferFile = GenerateFileForSessionLog();

	UE_LOG(LogOpenAccessibilityAnalytics, Log, TEXT("Dumping Event Log To File."));

	if (!WriteBufferToFile())
	{
		UE_LOG(LogOpenAccessibilityAnalytics, Warning, TEXT("EventLog Dumping Failed."));
	}

	return true;
}

void FOpenAccessibilityAnalyticsModule::LogEvent(const TCHAR* EventTitle, const TCHAR* LogString, ...)
{
	va_list Args;

	va_start(Args, LogString);
	TStringBuilder<1024> Message;
	Message.AppendV(LogString, Args);
	va_end(Args);

	EventBuffer.Add(
		LoggedEvent(EventTitle, *Message)
	);
}

FString FOpenAccessibilityAnalyticsModule::GenerateFileForSessionLog()
{
	FDateTime CurrentDateTime = FDateTime::Now();

	FString CombinedFileName = CurrentDateTime.ToString() + TEXT("- OpenAccessibility Event Log");
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + TEXT("Logs/OpenAccessibility/") + CombinedFileName);
}

bool FOpenAccessibilityAnalyticsModule::WriteBufferToFile()
{
	if (EventBuffer.IsEmpty())
		return false;

	FString CombindedString = FString("");
	LoggedEvent CurrEvent;
	while (!EventBuffer.IsEmpty())
	{
		CurrEvent = EventBuffer[0];
		EventBuffer.RemoveAt(0);

		CombindedString += FString::Printf(TEXT("| %s | - %s\r\n"), *CurrEvent.Title, *CurrEvent.Body);
	}

	if (FFileHelper::SaveStringToFile(
			CombindedString, 
			*SessionBufferFile, 
			FFileHelper::EEncodingOptions::AutoDetect, 
			&IFileManager::Get(), 
			EFileWrite::FILEWRITE_Append
	))
	{

		return true;
	}

	return false;
}

void FOpenAccessibilityAnalyticsModule::EnableDumpTick()
{
	const double DumpDelayCheck = 20.0f;

	FTickerDelegate TickDelegate = FTickerDelegate::CreateRaw(this, &FOpenAccessibilityAnalyticsModule::DumpTick);
	DumpTickHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate, DumpDelayCheck);
}

void FOpenAccessibilityAnalyticsModule::DisableDumpTick()
{
	if (DumpTickHandle.IsValid())
		FTSTicker::GetCoreTicker().RemoveTicker(DumpTickHandle);
}

void FOpenAccessibilityAnalyticsModule::AddConsoleCommands()
{
	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("OpenAccessibilityAnalytics.Debug.Add_Mock_Event"),
		TEXT("Adds a MOCK Event to the Eventbuffer"),

		FConsoleCommandWithArgsDelegate::CreateLambda(
			[this](const TArray<FString>& Args) {

				if (!(Args.Num() >= 2))
					return;

				FString EventTitle = Args[0];
				FString EventBody;

				for (int i = 1; i < Args.Num(); i++)
				{
					EventBody += Args[i] + TEXT(" ");
				}

				this->LogEvent(*EventTitle, *EventBody);
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