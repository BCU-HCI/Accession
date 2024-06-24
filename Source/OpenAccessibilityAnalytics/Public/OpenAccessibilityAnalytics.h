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

	bool DumpTick(float DeltaTime);

	// Analytics Logging

	void LogEvent(const TCHAR* EventTitle, const TCHAR* LogString, ...);

private:

	FString GenerateFileForSessionLog();

	bool WriteBufferToFile();

	void EnableDumpTick();

	void DisableDumpTick();

	void AddConsoleCommands();

	void RemoveConsoleCommands();

private:

	// Analytics Dumping

	FString SessionBufferFile;

	struct LoggedEvent
	{
	public:

		LoggedEvent()
		{ };

		LoggedEvent(const TCHAR* EventTitle, const TCHAR* EventString)
			: Title(EventTitle)
			, Body(EventString)
		{ };
		
		LoggedEvent(const FString& EventTitle, const FString& EventString)
			: Title(EventTitle)
			, Body(EventString)
		{ };

	public:
		FString Title;
		FString Body;
	};

	TArray<LoggedEvent> EventBuffer;

	FTSTicker::FDelegateHandle DumpTickHandle;

	// Console Commands

	TArray<IConsoleCommand*> ConsoleCommands;
};
