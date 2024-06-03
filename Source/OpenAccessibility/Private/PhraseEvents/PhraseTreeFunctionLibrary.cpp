#include "PhraseEvents/PhraseTreeFunctionLibrary.h"

#include "OpenAccessibility.h"
#include "OpenAccessibilityCommunication.h"

UPhraseTreeFunctionLibrary::UPhraseTreeFunctionLibrary(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer) 
{
	
}

TSharedRef<FPhraseTree> UPhraseTreeFunctionLibrary::GetPhraseTree() 
{
	FOpenAccessibilityCommunicationModule& OAComsModule = FOpenAccessibilityCommunicationModule::Get();
	  
	if (OAComsModule.PhraseTree.IsValid())
		return OAComsModule.PhraseTree.ToSharedRef();
	else return TSharedRef<FPhraseTree>();
}

TSharedRef<FAssetAccessibilityRegistry> UPhraseTreeFunctionLibrary::GetAssetRegistry() 
{
	FOpenAccessibilityModule& OAModule = FOpenAccessibilityModule::Get();

	if (OAModule.AssetAccessibilityRegistry.IsValid())
		return OAModule.AssetAccessibilityRegistry.ToSharedRef();
	else return TSharedRef<FAssetAccessibilityRegistry>();

	return TSharedRef<FAssetAccessibilityRegistry>();
}
