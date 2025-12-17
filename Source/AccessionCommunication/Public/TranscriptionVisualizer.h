// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "TranscriptionVisualizer.generated.h"

UCLASS()
class ACCESSIONCOMMUNICATION_API UTranscriptionVisualizer : public UObject
{
	GENERATED_BODY()

public:
	UTranscriptionVisualizer();
	virtual ~UTranscriptionVisualizer() override;

	void Initialize();

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
	UFUNCTION()
	void OnTranscriptionRecieved(TArray<FString> InTranscription);

protected:
	/// <summary>
	/// Gets the Position of the Visualizer for the Top Active Screen.
	/// </summary>
	/// <param name="OutPosition"></param>
	bool GetTopScreenVisualizerPosition(FVector2D &OutPosition);

	/// <summary>
	/// Gets the Position of the Visualizer for the Last Active Display.
	/// </summary>
	/// <param name="OutPosition"></param>
	bool GetDisplayVisualizerPosition(FVector2D &OutPosition);

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
	TWeakPtr<class STranscriptionVis> VisContent;

	const int32 MaxVisualisationAmount = 2;

	TArray<FString> TranscriptionList;
	FCriticalSection TranscriptionQueueMutex;
	bool bIsTranscriptionDirty = false;
};