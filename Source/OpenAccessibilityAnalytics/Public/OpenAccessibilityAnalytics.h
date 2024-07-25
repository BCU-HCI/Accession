// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#define OA_LOG(CategoryName, Verbosity, EventTitle, Format, ...) \
{ \
	UE_VALIDATE_FORMAT_STRING(Format, ##__VA_ARGS__); \
	UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__) \
	FOpenAccessibilityAnalyticsModule::Get().LogEvent(EventTitle, Format, ##__VA_ARGS__); \
}

class FOpenAccessibilityAnalyticsModule : public IModuleInterface {

public:

	/** IModuleInterface Implementation */

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override { return false; }

	/** End IModuleInterface Implementation */

	static FOpenAccessibilityAnalyticsModule& Get() 
	{
		return FModuleManager::GetModuleChecked<FOpenAccessibilityAnalyticsModule>("OpenAccessibilityAnalytics");
	}

	/**
	 * The Tick Event for Handling the Dump Event.
	 * @param DeltaTime Time since last Tick.
	 * @return 
	 */
	bool DumpTick(float DeltaTime);

	// Analytics Logging

	/**
	 * Logs the Event to the Analytics Module.
	 * @param EventTitle Title of the Log Event.
	 * @param LogString Body of the Log Event
	 * @param ... 
	 */
	void LogEvent(const TCHAR* EventTitle, const TCHAR* LogString, ...);

private:

	/**
	 * Creates a File Path for the current session of editor usage.
	 * @return The generated file path for this logging session.
	 */
	FString GenerateFileForSessionLog();

	/**
	 * Writes the Current Buffer of Logged Events to the Log File.
	 * @return True if the Buffer is successfully written to the Log File, otherwise False on Failure.
	 */
	bool WriteBufferToFile();

	/**
	 * Enables the Tick Delegate for Dumping the Event Buffer.
	 */
	void EnableDumpTick();

	/**
	 * Disables the Tick Delegate for Dumping the Event Buffer.
	 */
	void DisableDumpTick();

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
	FString SessionBufferFile;

	struct LoggedEvent
	{
	public:

		LoggedEvent()
		{ };

		LoggedEvent(const TCHAR* EventTitle, const TCHAR* EventString, FDateTime EventTimestamp = FDateTime::Now())
			: Title(EventTitle)
			, Body(EventString)
			, Timestamp(EventTimestamp)
		{ };
		
		LoggedEvent(const FString& EventTitle, const FString& EventString, FDateTime EventTimestamp = FDateTime::Now())
			: Title(EventTitle)
			, Body(EventString)
			, Timestamp(EventTimestamp)
		{ };

	public:
		FString Title;
		FString Body;

		FDateTime Timestamp;
	};

	/**
	 * Buffer of Currently Logged Events, to be moved to the Dump File.
	 */
	TArray<LoggedEvent> EventBuffer;

	FTSTicker::FDelegateHandle DumpTickHandle;

	// Console Commands

	/**
	 * Array of Registered Console Commands for the Open Accessibility Analytics Module.
	 */
	TArray<IConsoleCommand*> ConsoleCommands;
};


FORCEINLINE void FOpenAccessibilityAnalyticsModule::LogEvent(const TCHAR* EventTitle, const TCHAR* LogString, ...)
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