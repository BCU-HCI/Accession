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

void FPhraseTree::BindBranch(const TSharedPtr<FPhraseNode> InNode)
{
	ChildNodes.Add(InNode);
}

void FPhraseTree::ParseTranscription(TArray<FString> InTranscriptionSegments)
{
	if (InTranscriptionSegments.IsEmpty())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Provided Transcription is Empty ||"))
		return;
	}

	TArray<FString> SegmentWordArray = TArray<FString>();

	int SegmentCount = 0;
	for (FString& TranscriptionSegment : InTranscriptionSegments)
	{
		if (TranscriptionSegment.IsEmpty())
		{
			UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Transcription Segment is Empty ||"))
			continue;
		}

		// Filter the Transcription Segment, to remove any unwanted characters.
		TranscriptionSegment.ReplaceInline(TEXT("."), TEXT(" "), ESearchCase::IgnoreCase);
		TranscriptionSegment.TrimStartAndEndInline();

		// Parse the Transcription Segment into an Array of Words, removing any white space.
		TranscriptionSegment.ParseIntoArrayWS(SegmentWordArray, TEXT(","), true);
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

void FPhraseTree::ConstructPhraseTree()
{

}
