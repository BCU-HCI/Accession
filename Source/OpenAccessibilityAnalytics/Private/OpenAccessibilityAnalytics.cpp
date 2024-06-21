#include "OpenAccessibilityAnalytics.h"

#define LOCTEXT_NAMESPACE "FOpenAccessibilityAnalyticsModule"

void FOpenAccessibilityAnalyticsModule::StartupModule() 
{
	UE_LOG(LogTemp, Display, TEXT("Open Accessibility Analytics Module Start-Up"));
}

void FOpenAccessibilityAnalyticsModule::ShutdownModule() 
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOpenAccessibilityAnalyticsModule, OpenAccessibilityAnalytics)