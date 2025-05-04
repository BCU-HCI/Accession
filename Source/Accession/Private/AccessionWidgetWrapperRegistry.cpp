#include "AccessionWidgetWrapperRegistry.h"

#include "AccessionLogging.h"

UWidgetWrapperRegistry::~UWidgetWrapperRegistry()
{

	// Possible Widget Tick Clean-Up?
}

void UWidgetWrapperRegistry::Initialize()
{
	// Initialize Ticker
	FTSTicker& CoreTicker = FTSTicker::GetCoreTicker();
	TickDelegateHandle = CoreTicker.AddTicker(FTickerDelegate::CreateUObject(this, &UWidgetWrapperRegistry::Tick), 0.1f);
}

bool UWidgetWrapperRegistry::Tick(float delta)
{
	return false;
}

bool UWidgetWrapperRegistry::RegisterWidgetWrapper(const FName WidgetTypeToWrap, const WidgetWrapperFactory& WidgetWrapperFactory, bool ForceOverride)
{
	if (WidgetWrapperFactories.Contains(WidgetTypeToWrap))
	{
		UE_LOG(LogAccession, Warning, TEXT("Wrapper Already Registered for Widget: %s"), *WidgetTypeToWrap.ToString())

		if (!ForceOverride)
			return false;
	}

	WidgetWrapperFactories.Add(WidgetTypeToWrap, WidgetWrapperFactory);

	return true;
}

bool UWidgetWrapperRegistry::RegisterWidgetWrapper(const TSharedRef<SWidget>& WidgetToWrap, const WidgetWrapperFactory& WidgetWrapperFactory, bool ForceOverride)
{
	if (WidgetWrapperFactories.Contains(WidgetToWrap->GetType()))
	{
		UE_LOG(LogAccession, Warning, TEXT("Wrapper Already Registered for Widget: %s"), *WidgetToWrap->GetTypeAsString())

		if (!ForceOverride)
			return false;
	}

	WidgetWrapperFactories.Add(WidgetToWrap->GetType(), WidgetWrapperFactory);

	return true;
}

bool UWidgetWrapperRegistry::UnregisterWidgetWrapper(const FName WidgetTypeToRemove)
{
	return WidgetWrapperFactories.Remove(WidgetTypeToRemove) > 0;
}

bool UWidgetWrapperRegistry::UnregisterWidgetWrapper(const TSharedRef<SWidget>& WidgetToRemove)
{
	return WidgetWrapperFactories.Remove(WidgetToRemove->GetType()) > 0;
}

bool UWidgetWrapperRegistry::CreateWidgetWrapper(const TSharedRef<SWidget>& WidgetToWrap, UWidgetWrapper* WidgetWrapper)
{
	if (!WidgetWrapperFactories.Contains(WidgetToWrap->GetType()))
	{
		UE_LOG(LogAccession, Warning, TEXT("No Widget Wrapper Factory Found for Widget: %s"), *WidgetToWrap->GetTypeAsString())
		return false;
	}

	WidgetWrapper = WidgetWrapperFactories[WidgetToWrap->GetType()](WidgetToWrap);
	WidgetWrapper->Initialize(WidgetToWrap);

	// Add to List for Tick Propagation
	CreatedWidgetWrappers.Add(WidgetWrapper);

	return true;
}
