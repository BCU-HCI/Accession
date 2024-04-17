// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IMenu.h"

struct UAccessibilityContextMenu
{
public:
	UAccessibilityContextMenu();
	UAccessibilityContextMenu(const IMenu* Menu);

	~UAccessibilityContextMenu();

	bool Tick(float DeltaTime);

private:

	// const TSharedPtr<FWidgetIndexer> WidgetIndexer;
};