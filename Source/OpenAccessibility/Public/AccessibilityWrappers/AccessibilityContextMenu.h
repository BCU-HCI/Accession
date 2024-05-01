// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Framework/Application/IMenu.h"

#include "Indexer/Indexer.h"

#include "AccessibilityContextMenu.generated.h"

UCLASS(Abstract)
class UAccessibilityContextMenu : public UObject
{
	GENERATED_BODY()

public:

	UAccessibilityContextMenu() = default;
	UAccessibilityContextMenu(TSharedRef<IMenu> Menu);

	virtual ~UAccessibilityContextMenu();

	virtual void Init(TSharedRef<IMenu> InMenu);

	virtual bool Tick(float DeltaTime) { return true; };

	virtual bool Close() 
	{
		DestroyTicker();
		Menu.Pin()->Dismiss();

		return true;
	};

	void DestroyTicker();

	virtual void SetMenu(TSharedRef<IMenu> InMenu)
	{
		Menu = InMenu;
	}

protected:

	TSharedPtr<SWindow> GetWindow()
	{
		Menu.Pin()->GetOwnedWindow();
	}

public:
	TWeakPtr<IMenu> Menu;
	TWeakPtr<SWindow> Window;

private:

	// Ticker Components

	FTickerDelegate TickDelegate;
	FTSTicker::FDelegateHandle TickDelegateHandle;
};