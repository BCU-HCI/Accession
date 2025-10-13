// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#include "PhraseTreeUtils.h"

#include "AccessionComLogging.h"

UPhraseTreeUtils::UPhraseTreeUtils()
{
}

UPhraseTreeUtils::~UPhraseTreeUtils()
{
}

void UPhraseTreeUtils::RegisterFunctionLibrary(UPhraseTreeFunctionLibrary *LibraryToRegister)
{
	TSharedPtr<FPhraseTree> PhraseTreeSP = PhraseTree.Pin();
	if (!PhraseTreeSP.IsValid())
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("Cannot Register Phrase Tree Function Library Due To InValid Phrase Tree Reference."));
		return;
	}

	// For some reason this needs to be told directly to be kept alive,
	// even though it is a UPROPERTY TArray and should be kept alive by the UObject system.
	LibraryToRegister->AddToRoot();
	LibraryToRegister->BindBranches(PhraseTreeSP.ToSharedRef());
}
