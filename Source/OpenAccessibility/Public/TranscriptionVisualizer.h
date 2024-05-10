// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class OPENACCESSIBILITY_API FTranscriptionVisualizer
{
public:

	FTranscriptionVisualizer();
	~FTranscriptionVisualizer();

	virtual bool Tick(float DeltaTime);

	// Visualizer Methods

	void ConstructVisualizer();

	void UpdateVisualizer();

	void ReparentWindow();

	void MoveVisualizer();

protected:

	// Ticker Manager Methods

	void RegisterTicker();

	void UnregisterTicker();


protected:

	// Ticker Vars

	FTSTicker::FDelegateHandle TickDelegateHandle;

	// Vis Components

	TWeakPtr<SWindow> VisWindow;
	TWeakPtr<SWidget> VisContent;
};