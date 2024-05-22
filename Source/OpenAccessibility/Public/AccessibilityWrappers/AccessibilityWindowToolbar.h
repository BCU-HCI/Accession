// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AccessibilityWindowToolbar.generated.h"

UCLASS()
class OPENACCESSIBILITY_API UAccessibilityWindowToolbar : public UObject
{
	GENERATED_BODY()

public:

	UAccessibilityWindowToolbar();

	virtual ~UAccessibilityWindowToolbar();

	void Init();

	bool Tick(float DeltaTime);

	void ApplyToolbarIndexing(TSharedRef<SWindow> WindowToApply);

	// void ShowToolbarIndexing(TSharedRef<SWindow> WindowToShow);

	// void HideToolbarIndexing(TSharedRef<SWindow> WindowToHide);

private:



	void BindTicker();

	void UnbindTicker();

public:

private:

	TWeakPtr<SWindow> TargetWindow;

	// TUniquePtr<FIndexer> Indexer;

	TSet<SWindow*> IndexedWindows;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};