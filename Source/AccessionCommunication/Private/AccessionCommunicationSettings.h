// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "AccessionCommunicationSettings.generated.h"

UENUM()
enum class ETransctiptionActivation
{
	ENABLE_DISABLE UMETA(DisplayName = "Enable/Disable Upon Press"),
	PUSH_TO_TALK UMETA(DisplayName = "Push To Talk"),
};

UCLASS(config = MySettings)
class UAccessionCommunicationSettings : public UObject
{
	GENERATED_BODY()

public:

	UAccessionCommunicationSettings(const FObjectInitializer& obj);

	// Transcription Settings
	UPROPERTY(Config, EditAnywhere, Category = "Accession|Transcription")
	ETransctiptionActivation TranscriptionActivation;

	UPROPERTY(Config, EditAnywhere, Category = "Accession|Transcription")
	bool ActivationRequires;

};