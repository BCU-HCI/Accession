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

	/// <summary>
	/// Constructs the Visualizer Window, and Its Content.
	/// </summary>
	void ConstructVisualizer();

	/// <summary>
	/// Updates the Visualizer Window, If Active.
	/// </summary>
	void UpdateVisualizer();

	/// <summary>
	/// Reparents the Visualizer Window to the Active Window.
	/// </summary>
	void ReparentWindow();

	/// <summary>
	/// Moves the Visualizer Window to the Active Window Position.
	/// </summary>
	void MoveVisualizer();

	/// <summary>
	/// Callback for when Transcriptions are Recieved From Transcribed Audio.
	/// </summary>
	/// <param name="InTranscription">Incoming Array of Transcription Strings.</param>
	void OnTranscriptionRecieved(TArray<FString> InTranscription);

protected:

	/// <summary>
	/// Gets the Position of the Visualizer for the Top Active Screen.
	/// </summary>
	/// <param name="OutPosition"></param>
	bool GetTopScreenVisualizerPosition(FVector2D& OutPosition);

	/// <summary>
	/// Gets the Position of the Visualizer for the Last Active Display.
	/// </summary>
	/// <param name="OutPosition"></param>
	bool GetDisplayVisualizerPosition(FVector2D& OutPosition);

	// Ticker Manager Methods

	/// <summary>
	/// Registers the Ticker for the Visualizer.
	/// </summary>
	void RegisterTicker();

	/// <summary>
	/// Unregisters the Ticker for the Visualizer.
	/// </summary>
	void UnregisterTicker();


protected:

	// Ticker Vars

	FTSTicker::FDelegateHandle TickDelegateHandle;

	// Vis Components

	/// <summary>
	/// The Visualizers Containing Window.
	/// </summary>
	TWeakPtr<SWindow> VisWindow;

	/// <summary>
	/// The Content of the Visualizer Window.
	/// </summary>
	TWeakPtr<class SAccessibilityTranscriptionVis> VisContent;
};