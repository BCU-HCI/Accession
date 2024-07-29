// Copyright F-Dudley. All Rights Reserved.


#include "PhraseTree.h"
#include "PhraseTree/PhraseNode.h"
#include "Algo/Reverse.h"

#include "Logging/StructuredLog.h"
#include "OpenAccessibilityComLogging.h"
#include "OpenAccessibilityAnalytics.h"

FPhraseTree::FPhraseTree() : FPhraseNode(TEXT("ROOT_NODE"))
{
	ContextManager = FPhraseTreeContextManager();

	FTickerDelegate TickDelegate = FTickerDelegate::CreateRaw(this, &FPhraseTree::Tick);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);
}

FPhraseTree::~FPhraseTree()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

bool FPhraseTree::Tick(float DeltaTime)
{
	// Filter InActive Context Objects out of the stack.
	ContextManager.FilterContextStack();

	return true;
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

	// Loop over any Transcription Segments.
	for (FString& TranscriptionSegment : InTranscriptionSegments)
	{
		if (TranscriptionSegment.IsEmpty())
		{
			UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Transcription Segment is Empty ||"))
			continue;
		}

		// Filter the Transcription Segment, to remove any unwanted characters.
		TranscriptionSegment.TrimStartAndEndInline();
		TranscriptionSegment.ReplaceInline(TEXT("."), TEXT(""), ESearchCase::IgnoreCase);
		TranscriptionSegment.ReplaceInline(TEXT(","), TEXT(""), ESearchCase::IgnoreCase);
		TranscriptionSegment.ToUpperInline();

		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Filtered Transcription Segment: { %s } ||"), *TranscriptionSegment)

		// Parse the Transcription Segment into an Array of Words, removing any white space.
		TranscriptionSegment.ParseIntoArrayWS(SegmentWordArray);
		if (SegmentWordArray.Num() == 0)
		{
			UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Phrase Tree || Transcription Segment has no Word Content ||"))
			continue;
		}

		Algo::Reverse(SegmentWordArray);

		// Loop until the Segment is Empty
		while (!SegmentWordArray.IsEmpty())
		{

			FParseRecord ParseRecord = FParseRecord(ContextManager.GetContextStack());
			FParseResult ParseResult = ParsePhrase(SegmentWordArray, ParseRecord);

			ContextManager.UpdateContextStack(ParseRecord.ContextObjectStack);

			UE_LOGFMT(LogOpenAccessibilityCom, Log, "|| Phrase Tree || Segment: {0} | Result: {1} ||", SegmentCount, ParseResult.Result);

			switch (ParseResult.Result)
			{
				case PHRASE_PARSED:
				case PHRASE_PARSED_AND_EXECUTED:
				{
					OA_LOG(LogOpenAccessibilityCom, Log, TEXT("PhraseTree Propagation"), TEXT("{Success} Phrase Tree Parsed Correctly (%s)"),
						*ParseRecord.GetPhraseString())

					LastVistedNode.Reset();
					LastVistedParseRecord = FParseRecord();

					break;
				}

				case PHRASE_REQUIRES_MORE:
				{
					OA_LOG(LogOpenAccessibilityCom, Log, TEXT("PhraseTree Propagation"), TEXT("{Failed} Phrase Tree Propagation Requires More Segments. (%s)"), 
						*ParseRecord.GetPhraseString());

					// Store Reach Nodes, and the ParseRecord for future propagation attempts.
					LastVistedNode = ParseResult.ReachedNode;
					LastVistedParseRecord = ParseRecord;
				}

				case PHRASE_REQUIRES_MORE_CORRECT_PHRASES:
				{
					OA_LOG(LogOpenAccessibilityCom, Log, TEXT("PhraseTree Propagation"), TEXT("{Failed} Phrase Tree Propagation Requires More Correct Segments. (%s)"),
						*ParseRecord.GetPhraseString())

					LastVistedNode = ParseResult.ReachedNode;
					LastVistedParseRecord = ParseRecord;

					// Dirty Way of Ensuring all Segments in Transcription are Attempted.
					if (!SegmentWordArray.IsEmpty())  
						SegmentWordArray.Pop();

					break;
				}

				default:
				case PHRASE_UNABLE_TO_PARSE:
				{
					OA_LOG(LogOpenAccessibilityCom, Log, TEXT("PhraseTree Propagation"), TEXT("{Failed} Phrase Tree Propagation Failed. (%s)"),
						*ParseRecord.GetPhraseString())

					// Dirty Way of Ensuring all Segments in Transcription are Attempted.
					if (!SegmentWordArray.IsEmpty())
						SegmentWordArray.Pop();

					break;
				}
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
		TArray<FString> PhraseWordArrayCopy = TArray(InPhraseWordArray);

		FParseResult ParseResult = LastVistedNode->ParseChildren(PhraseWordArrayCopy, LastVistedParseRecord);
		if (ParseResult.Result == PHRASE_PARSED)
		{
			LastVistedNode.Reset();
			InParseRecord = LastVistedParseRecord;
			LastVistedParseRecord = FParseRecord();

			return ParseResult;
		}
		else if (ParseResult.Result != PHRASE_UNABLE_TO_PARSE)
		{ 
			return ParseResult;
		}
	}

	// Check if the Context Stack has Objects, if so propagation from the Context Root.
	if (ContextManager.HasContextObjects())
	{
		// Propagate from the Context Root, that is the Top of the Context Stack.
		return ContextManager.PeekContextObject()->GetContextRoot()->ParsePhraseAsContext(InPhraseWordArray, InParseRecord);
	}

	// Otherwise, start a new propagation entirely from the Tree Root.
	return ParseChildren(InPhraseWordArray, InParseRecord);
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

		// If the Start Node has no similar children, then bind the branch to the start node.
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