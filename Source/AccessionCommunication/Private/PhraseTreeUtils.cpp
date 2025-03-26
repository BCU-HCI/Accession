#include "PhraseTreeUtils.h"

#include "OpenAccessibilityComLogging.h"

UPhraseTreeUtils::UPhraseTreeUtils()
{

}

UPhraseTreeUtils::~UPhraseTreeUtils()
{

}

void UPhraseTreeUtils::RegisterFunctionLibrary(UPhraseTreeFunctionLibrary* LibraryToRegister)
{
	TSharedPtr<FPhraseTree> PhraseTreeSP = PhraseTree.Pin();
	if (!PhraseTreeSP.IsValid())
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("Cannot Register Phrase Tree Function Library Due To InValid Phrase Tree Reference."));
		return;
	}

	// For some reason this needs to be told directly to be kept alive,
	// even though it is a UPROPERTY TArray and should be kept alive by the UObject system.
	LibraryToRegister->AddToRoot();
	LibraryToRegister->BindBranches(PhraseTreeSP.ToSharedRef());
}
