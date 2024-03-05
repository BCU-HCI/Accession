// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAccessibility.h"
#include "AssetAccessibilityRegistry.h"
#include "PhraseTree.h"
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
}

void FOpenAccessibilityModule::ShutdownModule()
{
	UE_LOG(LogOpenAccessibility, Display, TEXT("OpenAccessibilityModule::ShutdownModule()"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenAccessibilityModule, OpenAccessibility)