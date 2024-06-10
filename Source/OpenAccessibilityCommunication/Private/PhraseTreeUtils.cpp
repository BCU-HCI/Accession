#include "PhraseTreeUtils.h"

#include "OpenAccessibilityComLogging.h"

UPhraseTreeUtils::UPhraseTreeUtils()
{

}

UPhraseTreeUtils::~UPhraseTreeUtils()
{

}

void UPhraseTreeUtils::RegisterFunctionLibrary(const UPhraseTreeFunctionLibrary* LibraryToRegister)
{
	TSharedPtr<FPhraseTree> PhraseTreeSP = PhraseTree.Pin();
	if (!PhraseTreeSP.IsValid())
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("Cannot Register Phrase Tree Function Library Due To InValid Phrase Tree Reference."));
		return;
	}

	LibraryToRegister->BindBranches(PhraseTreeSP.ToSharedRef());
}
