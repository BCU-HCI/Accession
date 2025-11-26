// Copyright (C) HCI-BCU 2025. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AccessionCommunicationSubsystem.h"
#include "../Public/AccessionCommunicationSubsystem.h"

#include "FrameWork/Application/IInputProcessor.h"

class UAccessionCommunicationSubsystem;

class ACCESSIONCOMMUNICATION_API FAccessionCommunicationInputProcessor : public IInputProcessor
{
public:

	virtual const TCHAR* GetDebugName() const override { return TEXT("AccessionCommunicationCommandInput"); }

	FAccessionCommunicationInputProcessor(const UAccessionCommunicationSubsystem* ACSubsystem)
		: WeakACSubsystem(ACSubsystem)
	{

	}

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { };


	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		if (InKeyEvent.IsRepeat())
			return false;

		if (const UAccessionCommunicationSubsystem* ACSubsystem = WeakACSubsystem.Get())
		{
			return ACSubsystem->HandleKeyDownEvent(InKeyEvent);
		}

		return false;
	}

	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		if (InKeyEvent.IsRepeat())
			return false;

		if (const UAccessionCommunicationSubsystem* ACSubsystem = WeakACSubsystem.Get())
		{
			return ACSubsystem->HandleKeyUpEvent(InKeyEvent);
		}

		return false;
	}

private:

	TWeakObjectPtr<const UAccessionCommunicationSubsystem> WeakACSubsystem;

};

