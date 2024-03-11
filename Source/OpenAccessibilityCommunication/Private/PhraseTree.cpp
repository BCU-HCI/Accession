// Copyright F-Dudley. All Rights Reserved.


#include "PhraseTree.h"
#include "PhraseTree/PhraseNode.h"

#include "OpenAccessibilityComLogging.h"

FPhraseTree::FPhraseTree()
{
	//RootNode = MakeShared<FPhraseNode>();
}

FPhraseTree::~FPhraseTree()
{
	//RootNode.Reset();
	NodeCount = NULL;
}

FParseResult FPhraseTree::ParseTranscription(const FString InPhrase)
{
	if (InPhrase.IsEmpty())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Provided Phrase is Empty ||"))

		return FParseResult(PHRASE_NOT_PARSED);
	}

	if (ChildNodes.Num() == 0)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Phrase Tree || Current Phrase Tree has no nodes. ||"))
		return FParseResult(PHRASE_NOT_PARSED);
	}

	TArray<FString> SegmentedPhraseArray;
	InPhrase.ParseIntoArray(SegmentedPhraseArray, TEXT(" "), true);

	if (SegmentedPhraseArray.Num() == 0)
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Provided Phrase After Segmenting is Empty ||"))

		return FParseResult(PHRASE_NOT_PARSED);
	}

	FParseResult ParseResult;
	FParseRecord ParseRecord = FParseRecord();

	// First Check any previous Node that was reached,
	// as it may be a continuation of the previous phrase.
	if (LastVistedNode != nullptr)
	{
		ParseResult = LastVistedNode->ParsePhrase(SegmentedPhraseArray, ParseRecord);

		if (ParseResult.Result == PHRASE_PARSED)
		{
			UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Phrase Parsed from Previous Phrase ||"))

			LastVistedNode = nullptr;
			return ParseResult;
		}
	}

	// If the Last Visted Node is not valid, then we will start from the Root Node.
	ParseResult = ParsePhrase(SegmentedPhraseArray, ParseRecord);
	if (ParseResult.Result == PHRASE_UNABLE_TO_PARSE || ParseResult.Result == PHRASE_NOT_PARSED)
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Phrase Cannot be Parsed ||"))
	}
	else
	{
		LastVistedNode = ParseResult.ReachedNode;
	}

	return ParseResult;

}

void FPhraseTree::BindBranch(const TSharedPtr<FPhraseNode> InNode)
{
	ChildNodes.Add(InNode);
}

void FPhraseTree::ConstructPhraseTree()
{

}
