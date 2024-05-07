// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Framework/Application/IMenu.h"

#include "PhraseTree/Containers/ContextObject.h"

#include "ContextMenuObject.generated.h"

UCLASS()
class OPENACCESSIBILITYCOMMUNICATION_API UPhraseTreeContextMenuObject : public UPhraseTreeContextObject
{
	GENERATED_BODY()

public:

	UPhraseTreeContextMenuObject();
	UPhraseTreeContextMenuObject(TSharedRef<IMenu> Menu);

	virtual ~UPhraseTreeContextMenuObject();

	virtual void Init(TSharedRef<IMenu> InMenu);
	virtual void Init(TSharedRef<IMenu> InMenu, TSharedRef<FPhraseNode> InContextRoot);

	virtual bool Tick(float DeltaTime) { return true; };

	virtual bool Close() 
	{
		RemoveTickDelegate();
		Menu.Pin()->Dismiss();

		return true;
	};

	void BindTickDelegate();
	void RemoveTickDelegate();

	void BindMenuDismissed(TSharedRef<IMenu> InMenu);
	void RemoveMenuDismissed(TSharedRef<IMenu> InMenu);

	virtual void SetMenu(TSharedRef<IMenu> InMenu)
	{
		Menu = InMenu;
	}

	virtual void ScaleMenu(const float ScaleFactor) {};

protected:

	TSharedPtr<SWindow> GetWindow()
	{
		return Menu.Pin()->GetOwnedWindow();
	}

	void OnMenuDismissed(TSharedRef<IMenu> Menu);

private:

public:
	TWeakPtr<IMenu> Menu;
	TWeakPtr<SWindow> Window;

private:

	// Ticker Components

	FTickerDelegate TickDelegate;
	FTSTicker::FDelegateHandle TickDelegateHandle;

	FDelegateHandle MenuDismissedHandle;
};