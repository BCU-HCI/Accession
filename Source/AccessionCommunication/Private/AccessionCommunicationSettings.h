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

USTRUCT(BlueprintType)
struct FActivationKeys
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	TSet<FKey> Keys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	bool bShift;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	bool bCtrl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	bool bAlt;

	FActivationKeys()
		: bShift(false), bCtrl(false), bAlt(false)
	{
		Keys = {
			FKey(EKeys::LeftAlt),
		};
	}

	FActivationKeys(const TSet<FKey>& InKeys)
	{
		Keys = InKeys;
		bShift = false;
		bCtrl = false;
		bAlt = false;

	}

	bool operator==(const FKeyEvent& event) const
	{
		FKey EventKey = event.GetKey();

		if (!Keys.Contains(EventKey))
			return false;

		if (bShift != event.IsShiftDown() 
			&& !(EventKey == EKeys::LeftAlt || EventKey == EKeys::RightAlt))
			return false;

		if (bCtrl != event.IsControlDown()
			&& !(EventKey == EKeys::LeftControl || EventKey == EKeys::RightControl))
			return false;

		if (bAlt != event.IsAltDown()
			&& !(EventKey == EKeys::LeftAlt || EventKey == EKeys::RightAlt))
			return false;

		return true;
	}

	bool operator!=(const FKeyEvent& event) const
	{
		return !(*this == event);
	}
};

UCLASS(config = MySettings)
class UAccessionCommunicationSettings : public UObject
{
	GENERATED_BODY()

public:

	UAccessionCommunicationSettings(const FObjectInitializer& obj)
	{
		TranscriptionActivation = ETransctiptionActivation::ENABLE_DISABLE;

		ActivationKeys = FActivationKeys({
			EKeys::LeftAlt,
			EKeys::RightAlt
		});

		bSaveTemporaryWAV = false;

		// Command Propagation
		CommandConfidenceThreshold = 2;

	}

	// Activation Settings
	UPROPERTY(Config, EditAnywhere, Category = "Accession|Activation")
	ETransctiptionActivation TranscriptionActivation;

	UPROPERTY(Config, EditAnywhere, Category = "Accession|Activation")
	FActivationKeys ActivationKeys;

	UPROPERTY(Config, EditAnywhere, Category = "Accession|Audio", DisplayName = "Save Temporary WAV File")
	bool bSaveTemporaryWAV;

	// Command Settings
	UPROPERTY(Config, EditAnywhere, Category = "Accession|Commands", meta = (ClampMin = 0, ClampMax = 10))
	int32 CommandConfidenceThreshold;

	// Visualiser Settings

};