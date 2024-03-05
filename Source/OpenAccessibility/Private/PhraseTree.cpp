// Copyright F-Dudley. All Rights Reserved.


#include "PhraseTree.h"
#include "PhraseTree/PhraseNode.h"

#include "OpenAccessibilityLogging.h"

FPhraseTree::FPhraseTree()
{
	RootNode = MakeShared<FPhraseNode>();
}

FPhraseTree::~FPhraseTree()
{
	LastVistedNode.Reset();
	RootNode.Reset();
	NodeCount = NULL;
}

FPhrasePropogationResult FPhraseTree::ParsePhrase(const FString InPhrase)
{
	if (InPhrase.IsEmpty())
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("|| Phrase Tree || Provided Phrase is Empty ||"))

		return FPhrasePropogationResult(PHRASE_CANNOT_BE_PARSED);
	}

	TArray<FString> SegmentedPhraseArray;
	InPhrase.ParseIntoArray(SegmentedPhraseArray, TEXT(" "), true);

	if (SegmentedPhraseArray.Num() == 0)
	{
		UE_LOG(LogOpenAccessibility, Log, TEXT("|| Phrase Tree || Provided Phrase After Segmenting is Empty ||"))

		return FPhrasePropogationResult(PHRASE_CANNOT_BE_PARSED);
	}

	return RootNode->ParsePhrase(SegmentedPhraseArray);
}

void FPhraseTree::BindBranch(const TSharedPtr<FPhraseNode> InNode)
{
	RootNode->BindChildNode(InNode);
}

void FPhraseTree::ConstructPhraseTree()
{

}
