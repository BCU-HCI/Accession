// Copyright F-Dudley. All Rights Reserved.


#include "PhraseTree.h"
#include "PhraseTree/PhraseNode.h"
#include "Algo/Reverse.h"

#include "Logging/StructuredLog.h"
#include "OpenAccessibilityComLogging.h"

FPhraseTree::FPhraseTree() : FPhraseNode(TEXT("ROOT_NODE"))
{

}

FPhraseTree::~FPhraseTree()
{

}


void FPhraseTree::ParseTranscription(TArray<FString> InTranscriptionSegments)
{
	if (InTranscriptionSegments.IsEmpty())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Provided Transcription is Empty ||"))
		return;
	}

	TArray<FString> SegmentWordArray = TArray<FString>();
	const TCHAR* ParseDelimsArray[11] = {
		// Whitespace Delims
		TEXT(" "),
		TEXT("\t"),
		TEXT("\r"),
		TEXT("\n"),
		TEXT(""),

		// Punctuation Delims
		TEXT("."),
		TEXT(","),
		TEXT("!"),
		TEXT("?"),
		TEXT(":"),
		TEXT(";"),
	};
	int ParseDelimsCount = UE_ARRAY_COUNT(ParseDelimsArray);

	int SegmentCount = 0;
	for (FString& TranscriptionSegment : InTranscriptionSegments)
	{
		if (TranscriptionSegment.IsEmpty())
		{
			UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Transcription Segment is Empty ||"))
			continue;
		}

		// Filter the Transcription Segment, to remove any unwanted characters.
		TranscriptionSegment.TrimStartAndEndInline();
		TranscriptionSegment.ReplaceInline(TEXT(","), TEXT(""), ESearchCase::IgnoreCase);
		TranscriptionSegment.ToUpperInline();

		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Filtered Transcription Segment: { %s } ||"), *TranscriptionSegment)

		// Parse the Transcription Segment into an Array of Words, removing any white space.
		TranscriptionSegment.ParseIntoArrayWS(SegmentWordArray, TEXT("."), true);
		if (SegmentWordArray.Num() == 0)
		{
			UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Transcription Segment has no Word Content ||"))
			continue;
		}

		Algo::Reverse(SegmentWordArray);

		FParseRecord ParseRecord = FParseRecord();
		FParseResult ParseResult = ParsePhrase(SegmentWordArray, ParseRecord);

		UE_LOGFMT(LogOpenAccessibilityCom, Log, "|| Phrase Tree || Segment: {0} | Result: {1} ||", SegmentCount, ParseResult.Result);

		switch (ParseResult.Result)
		{
			case PHRASE_PARSED:
			case PHRASE_PARSED_AND_EXECUTED:
			{
				UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Transcription Segment Parsed ||"))
				
				LastVistedNode.Reset();

				break;
			}

			case PHRASE_REQUIRES_MORE:
			{
				UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Transcription Segment Requires More Word Segments ||"))

				LastVistedNode = ParseResult.ReachedNode;

				break;
			}

			case PHRASE_UNABLE_TO_PARSE:
			{
				UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Transcription Segment Unable to be Parsed ||"))
				break;
			}
		}

		SegmentCount++;
		SegmentWordArray.Reset();
	}
}

FParseResult FPhraseTree::ParsePhrase(TArray<FString>& InPhraseWordArray, FParseRecord& InParseRecord)
{
	if (InPhraseWordArray.IsEmpty())
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Phrase Tree || Provided Transcription Segment is Empty ||"));

		return FParseResult(PHRASE_NOT_PARSED);
	}

	// First give the last visited node a chance to parse the phrase.
	// due to the possibility of connecting phrases over different transcription segments.
	if (LastVistedNode != nullptr && LastVistedNode.IsValid())
	{
		FParseResult ParseResult = LastVistedNode->ParsePhrase(InPhraseWordArray, InParseRecord);
		if (ParseResult.Result == PHRASE_PARSED || ParseResult.Result == PHRASE_PARSED_AND_EXECUTED)
		{
			LastVistedNode.Reset();

			return ParseResult;
		}
	}

	// Proceed to start a new propogation.
	for (const TSharedPtr<FPhraseNode>& ChildNode : ChildNodes)
	{
		if (!ChildNode->RequiresPhrase(InPhraseWordArray.Last()))
			continue;

		// Remove reference to Last Visted Node,
		// due to a new propogation being started.
		LastVistedNode.Reset();

		return ChildNode->ParsePhrase(InPhraseWordArray, InParseRecord);
	}

	UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Phrase Tree || No Parse Path Found ||"));

	return FParseResult(PHRASE_NOT_PARSED);
}


void FPhraseTree::BindBranch(const TPhraseNode& InNode)
{
	TArray<FPhraseTreeBranchBind> ToBindArray = TArray<FPhraseTreeBranchBind>();

	ToBindArray.Add(FPhraseTreeBranchBind(AsShared(), InNode));

	while (!ToBindArray.IsEmpty())
	{
		FPhraseTreeBranchBind BranchToBind = ToBindArray.Pop();

		// Check all ChildNodes to see if they are similar in purpose.
		for (auto& ChildNode : BranchToBind.StartNode->ChildNodes)
		{
			// If a ChildNode meets the same requirements as the BranchRoot,
			// then Split Bind Process to the ChildNodes.
			if (ChildNode->RequiresPhrase(BranchToBind.BranchRoot->BoundPhrase))
			{
				for (auto& BranchChildNode : BranchToBind.BranchRoot->ChildNodes)
				{
					ToBindArray.Add(FPhraseTreeBranchBind(ChildNode, BranchChildNode));
				}

				continue;
			}
		}

		// If the Start Node has no similar chilren, then bind the branch to the start node.
		// Can force bind, as previous checks show no child is similar.
		BranchToBind.StartNode->BindChildNodeForce(BranchToBind.BranchRoot);
	}
}

void FPhraseTree::BindBranches(const TPhraseNodeArray& InNodes)
{
	for (const TSharedPtr<FPhraseNode>& Node : InNodes)
	{
		BindBranch(Node);
	}
}