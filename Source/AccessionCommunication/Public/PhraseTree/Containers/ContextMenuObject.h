// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Framework/Application/IMenu.h"

#include "PhraseTree/Containers/ContextObject.h"

#include "ContextMenuObject.generated.h"

UCLASS()
class ACCESSIONCOMMUNICATION_API UPhraseTreeContextMenuObject : public UPhraseTreeContextObject
{
	GENERATED_BODY()

public:
	UPhraseTreeContextMenuObject();
	UPhraseTreeContextMenuObject(TSharedRef<IMenu> Menu);

	virtual ~UPhraseTreeContextMenuObject();

	/// <summary>
	/// Initializes the Context Menu Object.
	/// </summary>
	/// <param name="InMenu">- The Menu Object for this Context Menu.</param>
	virtual void Init(TSharedRef<IMenu> InMenu);

	/// <summary>
	/// Initializes the Context Menu Object.
	/// </summary>
	/// <param name="InMenu">- The Menu Object For this Context Menu.</param>
	/// <param name="InContextRoot">- The Context Root In The Phrase Tree For This Object.</param>
	virtual void Init(TSharedRef<IMenu> InMenu, TSharedRef<FPhraseNode> InContextRoot);

	virtual bool Tick(float DeltaTime) { return true; };

	/// <summary>
	/// Closes the Context Menu Object.
	/// </summary>
	/// <returns></returns>
	virtual bool Close() override
	{
		RemoveTickDelegate();
		Menu.Pin()->Dismiss();

		return true;
	};

	/// <summary>
	/// Binds the Tick Delegate to the Core Ticker.
	/// </summary>
	void BindTickDelegate();

	/// <summary>
	/// UnBinds the Tick Delegate from the Core Ticker.
	/// </summary>
	void RemoveTickDelegate();

	/// <summary>
	/// Binds the Menu Dismissed Callback to the Menu.
	/// </summary>
	/// <param name="InMenu"></param>
	void BindMenuDismissed(TSharedRef<IMenu> InMenu);

	/// <summary>
	/// UnBinds the Menu Dismissed Callback from the Menu.
	/// </summary>
	/// <param name="InMenu"></param>
	void RemoveMenuDismissed(TSharedRef<IMenu> InMenu);

	/// <summary>
	/// Sets the Menu Object for this Context Menu.
	/// </summary>
	/// <param name="InMenu"></param>
	virtual void SetMenu(TSharedRef<IMenu> InMenu)
	{
		Menu = InMenu;
	}

	/// <summary>
	/// Scales the Provided Menu Object, and any Key Objects.
	/// </summary>
	/// <param name="ScaleFactor"></param>
	virtual void ScaleMenu(const float ScaleFactor) {};

protected:
	/// <summary>
	/// Gets the Window Object for this Context Menu.
	/// </summary>
	/// <returns></returns>
	TSharedPtr<SWindow> GetWindow()
	{
		return Menu.Pin()->GetOwnedWindow();
	}

	/// <summary>
	/// Callback for the Dismissal of the Menu.
	/// </summary>
	/// <param name="Menu"></param>
	void OnMenuDismissed(TSharedRef<IMenu> Menu);

public:
	/// <summary>
	/// The Menu Object.
	/// </summary>
	TWeakPtr<IMenu> Menu;

	/// <summary>
	/// The Menu's Window.
	/// </summary>
	TWeakPtr<SWindow> Window;

private:
	// Ticker Components

	FTickerDelegate TickDelegate;
	FTSTicker::FDelegateHandle TickDelegateHandle;

	FDelegateHandle MenuDismissedHandle;
};