// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAccessibility.h"
#include "OpenAccessibilityCommunication.h"
#include "OpenAccessibilityLogging.h"

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

	// TEST OF REGISTRY
	//FOpenAccessibilityCommunicationModule::Get().OnTranscriptionRecieved
	//	.AddLambda([this](const TArray<FString> Transcription) {
	//		for (auto& Index : AssetAccessibilityRegistry->GetAllGraphKeyIndexes())
	//		{
	//			AssetAccessibilityRegistry->GraphAssetIndex[Index]->GetNode(0)->NodePosY += 100;
	//		}
	//	});
}

void FOpenAccessibilityModule::ShutdownModule()
{
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::ShutdownModule()"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenAccessibilityModule, OpenAccessibility)