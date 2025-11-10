// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "AccessionCommunicationSettings.h"

UAccessionCommunicationSettings::UAccessionCommunicationSettings(const FObjectInitializer& obj)
	: Super(obj)
{
	TranscriptionActivation = ETransctiptionActivation::ENABLE_DISABLE;
	ActivationRequires = false;
}