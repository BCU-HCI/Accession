// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

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