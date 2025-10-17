// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogVerbosity.h"

#define OA_LOG(CategoryName, Verbosity, EventTitle, Format, ...)                                      \
	{                                                                                                 \
		UE_VALIDATE_FORMAT_STRING(Format, ##__VA_ARGS__);                                             \
		UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__)                                        \
		FAccessionAnalyticsModule::Get().LogEvent(EventTitle, ELogLevel::Log, Format, ##__VA_ARGS__); \
	}

enum ELogLevel : int8
{
	Log = 0,
	Important = 1
};

struct FLoggedEvent
{
public:
	FLoggedEvent()
		: Level(ELogLevel::Log) {};

	FLoggedEvent(const TCHAR *EventTitle, const TCHAR *EventString, const ELogLevel Level = ELogLevel::Log, FDateTime EventTimestamp = FDateTime::Now())
		: Title(EventTitle), Body(EventString), Level(Level), Timestamp(EventTimestamp) {};

	FLoggedEvent(const FString &EventTitle, const FString &EventString, const ELogLevel Level = ELogLevel::Log, FDateTime EventTimestamp = FDateTime::Now())
		: Title(EventTitle), Body(EventString), Level(Level), Timestamp(EventTimestamp) {};

public:
	FString Title;
	FString Body;
	ELogLevel Level;

	FDateTime Timestamp;
};

class FAccessionAnalyticsModule : public IModuleInterface
{

public:
	/** IModuleInterface Implementation */

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override { return false; }

	/** End IModuleInterface Implementation */

	static FAccessionAnalyticsModule &Get()
	{
		return FModuleManager::GetModuleChecked<FAccessionAnalyticsModule>("AccessionAnalytics");
	}

	// Analytics Logging

	/**
	 * Logs the Accession Event to the Log File.
	 * @param EventTitle Title of the Event
	 * @param EventLogLevel Level of Logging for the Event.
	 * @param LogString Body of the Event.
	 */
	void LogEvent(const TCHAR *EventTitle, ELogLevel EventLogLevel, const TCHAR *LogString, ...)
	{
		va_list Args;

		va_start(Args, LogString);
		TStringBuilder<1024> Message;
		Message.AppendV(LogString, Args);
		va_end(Args);

		if (!WriteEventToFile(FLoggedEvent(EventTitle, *Message, EventLogLevel)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Error Occured During Event Log."))
		}
	}

private:
	/**
	 * Creates a File Path for the current session of editor usage.
	 * @return The generated file path for this logging session.
	 */
	FString GenerateFileForSessionLog();

	FString GenerateEventString(const FLoggedEvent &LoggedEvent);

	/**
	 * Writes the Provided Event to the Log File.
	 * @return True if the Event was Successfully Written to the File, False if there was an Error Logging.
	 */
	bool WriteEventToFile(const FLoggedEvent &LoggedEvent);

	bool WriteEventsToFile(const TArray<FLoggedEvent> &LoggedEvents);

	/**
	 * Binds all relevant Console Commands for the Open Accessibility Analytics Module.
	 */
	void AddConsoleCommands();

	/**
	 * Unbinds all relevant Console Commands for the Open Accessibility Analytics Module.
	 */
	void RemoveConsoleCommands();

private:
	// Analytics Dumping

	/**
	 * The Current File Path and File Name for this Current Logging Session.
	 */
	FString SessionLogFile;

	// Console Commands

	/**
	 * Array of Registered Console Commands for the Open Accessibility Analytics Module.
	 */
	TArray<IConsoleCommand *> ConsoleCommands;
};

FORCEINLINE FString FAccessionAnalyticsModule::GenerateEventString(const FLoggedEvent &LoggedEvent)
{
	return FString::Format(
		TEXT("[ {0} | {1} ] - {2} - {3}\n"),
		{LoggedEvent.Level, LoggedEvent.Timestamp.ToString(),
		 LoggedEvent.Title, LoggedEvent.Body});
}

FORCEINLINE bool FAccessionAnalyticsModule::WriteEventToFile(const FLoggedEvent &LoggedEvent)
{
	FString EventString = GenerateEventString(LoggedEvent);

	return FFileHelper::SaveStringToFile(
		EventString,
		*SessionLogFile,
		FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(),
		EFileWrite::FILEWRITE_Append);
}

FORCEINLINE bool FAccessionAnalyticsModule::WriteEventsToFile(const TArray<FLoggedEvent> &LoggedEvents)
{
	FString CombinedLogStrings;
	for (const FLoggedEvent &LoggedEvent : LoggedEvents)
	{
		CombinedLogStrings += GenerateEventString(LoggedEvent);
	}

	return FFileHelper::SaveStringToFile(
		CombinedLogStrings,
		*SessionLogFile,
		FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(),
		EFileWrite::FILEWRITE_Append);
}