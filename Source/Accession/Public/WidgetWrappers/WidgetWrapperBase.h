// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "WidgetWrapperBase.generated.h"

UCLASS(Abstract)
class UWidgetWrapper : public UObject
{
	GENERATED_BODY()

public:

	virtual ~UWidgetWrapper() override
	{
		
	}

	virtual void Initialize(const TSharedRef<SWidget>& InWidgetToWrap, bool AncestorWrapper = false)
	{
		WrappedWidget = InWidgetToWrap;
		bIsAncestorWrapper = AncestorWrapper;
	}

	virtual void Tick(float& DeltaTime) { }

    bool IsAncestorWrapper() const { return bIsAncestorWrapper; }

	bool IsActive() const { return bIsActive; }

private:

	TWeakPtr<SWidget> WrappedWidget;

	bool bIsActive = false;

	bool bIsAncestorWrapper = false;
};