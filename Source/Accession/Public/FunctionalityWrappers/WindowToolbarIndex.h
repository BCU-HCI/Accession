// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Indexers/Indexer.h"

#include "WindowToolbarIndex.generated.h"

/**
 * Wrapper for Indexing Window ToolBar Elements.
 */
UCLASS()
class ACCESSION_API UWindowToolbarIndex : public UObject
{
	GENERATED_BODY()

public:
	UWindowToolbarIndex();

	virtual ~UWindowToolbarIndex();

	bool Tick(float DeltaTime);

	// -- Parse Events --

	/**
	 * Selects the Active ToolBars Element, based on the provided Index.
	 * @param Index The Index of the ToolBar Element To Select.
	 */
	void SelectToolbarItem(int32 Index);

	// -- End of Parse Events --

	/**
	 * Checks to see if the Active Toolkit being Indexed is the provided Toolkit Widget.
	 * @param ToolkitWidget Toolkit Widget to Check if it is the active toolkit being Indexed.
	 * @return True if the provided toolkit is the active widget, otherwise False.
	 */
	bool IsActiveToolbar(const TSharedRef<SWidget> &ToolkitWidget);

	/**
	 * Gets the Stored Active Toolkit Widget.
	 * @return Shared Pointer to the Active Toolkit Widget, otherwise Invalid Pointer.
	 */
	TSharedPtr<SWidget> GetActiveToolkitWidget() const;

private:
	/**
	 * Applies Indexing to the Provided Toolkits ToolBar.
	 * @param ToolkitWidget The Toolkit to apply toolbar indexing to.
	 * @param ToolkitWindow The Window containing the provided Toolkit.
	 * @return
	 */
	bool ApplyToolbarIndexing(TSharedRef<SWidget> ToolkitWidget, TSharedRef<SWindow> ToolkitWindow);

	// Widget Getters

	/**
	 * Gets the Border Widget that houses the windows active toolkit.
	 * @param WindowToFindContainer The Window to Find the Container for Toolkit Widgets.
	 * @return A Shared Pointer of the Found Border Widget, otherwise an Invalid Shared Pointer.
	 */
	TSharedPtr<SBorder> GetWindowContentContainer(TSharedRef<SWindow> WindowToFindContainer);

	/**
	 * Gets the Toolkits Root ToolBar Widget.
	 * @param ToolKitWidget The ToolKit to search from.
	 * @param OutToolBar The Found ToolBar Root, otherwise an Invalid Shared Pointer.
	 * @return True if the ToolBar Root was Found, otherwise False.
	 */
	bool GetToolKitToolBar(TSharedRef<SWidget> ToolKitWidget, TSharedPtr<SWidget> &OutToolBar);

	/**
	 * Binds the Tick Method to the Applications Core Ticker.
	 */
	void BindTicker();

	/**
	 * UnBinds the Tick Method from the Applications Core Ticker.
	 */
	void UnbindTicker();

public:
private:
	/**
	 * Previously Found Top Active Window.
	 */
	TWeakPtr<SWindow> LastTopWindow;

	/**
	 * Previously Found Toolkit Parent Widget.
	 */
	TWeakPtr<SBorder> LastToolkitParent;

	/**
	 * Previously Found Active Toolkit Widget.
	 */
	TWeakPtr<SWidget> LastToolkit;

	/**
	 * Indexer for the Current Active ToolBar.
	 */
	FIndexer<int32, SMultiBlockBaseWidget *> ToolbarIndex;

	/**
	 * TickDelegate Handle for the Tick Method.
	 */
	FTSTicker::FDelegateHandle TickDelegateHandle;

	/**
	 * Array of Registered Console Commands for the Window Toolbar.
	 */
	TArray<IConsoleCommand *> ConsoleCommands;
};