// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EPhrasePositionalInput : uint8
{
	TOP,
	MIDDLE,
	BOTTOM,
	LEFT,
	RIGHT,
	CENTER
};

UENUM()
enum class EPhraseDirectionalInput : int8
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD
};

UENUM()
enum class EPhrase2DDirectionalInput : int8
{
	UP = EPhraseDirectionalInput::UP,
	DOWN = EPhraseDirectionalInput::DOWN,
	LEFT = EPhraseDirectionalInput::LEFT,
	RIGHT = EPhraseDirectionalInput::RIGHT,
};

UENUM()
enum class EPhraseScrollInput : uint8
{
	UP,	   // 0
	DOWN,  // 1
	TOP,   // 2
	BOTTOM // 3
};