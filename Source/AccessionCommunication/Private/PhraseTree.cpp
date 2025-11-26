// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "PhraseTree.h"
#include "PhraseTree/PhraseNode.h"
#include "Algo/Reverse.h"

#include "Logging/StructuredLog.h"
#include "AccessionComLogging.h"
#include "AccessionAnalytics.h"

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
		UE_LOG(LogAccessionCom, Log, TEXT("|| Phrase Tree || Provided Transcription is Empty ||"))
		return;
	}

	TArray<FString> SegmentWordArray = TArray<FString>();
	int SegmentCount = 0;

	// Loop over any Transcription Segments.
	for (FString &TranscriptionSegment : InTranscriptionSegments)
	{
		if (TranscriptionSegment.IsEmpty())
		{
			UE_LOG(LogAccessionCom, Log, TEXT("|| Phrase Tree || Transcription Segment is Empty ||"))
			continue;
		}

		if (!PrepareTranscriptionSegment(TranscriptionSegment, SegmentWordArray))
		{
			continue;
		}

		Algo::Reverse(SegmentWordArray);

		// Loop until the Segment is Empty
		while (!SegmentWordArray.IsEmpty())
		{

			FParseRecord ParseRecord = FParseRecord(ContextManager.GetContextStack());
			FParseResult ParseResult = ParsePhrase(SegmentWordArray, ParseRecord);

			ContextManager.UpdateContextStack(ParseRecord.ContextObjectStack);

			UE_LOGFMT(LogAccessionCom, Log, "|| Phrase Tree || Segment: {0} | Result: {1} ||", SegmentCount, ParseResult.Result);

			HandleParseResult(ParseResult, ParseRecord, SegmentWordArray);
		}

		SegmentCount++;
		SegmentWordArray.Reset();
	}
}

bool FPhraseTree::PrepareTranscriptionSegment(FString InSegment, TArray<FString>& OutWordArray)
{
	InSegment.TrimStartAndEndInline();
	InSegment.ReplaceInline(TEXT("."), TEXT(""), ESearchCase::IgnoreCase);
	InSegment.ReplaceInline(TEXT(","), TEXT(""), ESearchCase::IgnoreCase);
	InSegment.ToUpperInline();

	UE_LOG(LogAccessionCom, Log, TEXT("|| Phrase Tree || Filtered Transcription Segment: { %s } ||"), *InSegment)

	InSegment.ParseIntoArrayWS(OutWordArray);

	return OutWordArray.Num() > 0;
}

bool FPhraseTree::HandleParseResult(const FParseResult& InParseResult, FParseRecord& ParseRecord, TArray<FString>& WordsArray)
{
	switch (InParseResult.Result)
	{
		case PHRASE_PARSED:
		case PHRASE_PARSED_AND_EXECUTED:
		{
			OA_LOG(LogAccessionCom, Log, TEXT("PhraseTree"), TEXT("{Success} (%s)"), *ParseRecord.GetPhraseString());

			LastVistedNode.Reset();
			LastVistedParseRecord = FParseRecord();

			return true;
		}
		case PHRASE_REQUIRES_MORE:
		{
			OA_LOG(LogAccessionCom, Log, TEXT("PhraseTree"), TEXT("{Need More Input} (%s)"), *ParseRecord.GetPhraseString());

			LastVistedNode = InParseResult.ReachedNode;
			LastVistedParseRecord = ParseRecord;

			return false;
		}

		case PHRASE_REQUIRES_MORE_CORRECT_PHRASES:
		case PHRASE_UNABLE_TO_PARSE:
		default:
		{
			OA_LOG(LogAccessionCom, Log, TEXT("PhraseTree"), TEXT("{Failed} (%s)"), *ParseRecord.GetPhraseString());

			if (!WordsArray.IsEmpty())
				WordsArray.Pop();

			return true;
		}
	}

	return false;
}

FParseResult FPhraseTree::ParsePhrase(TArray<FString> &InPhraseWordArray, FParseRecord &InParseRecord)
{
	if (InPhraseWordArray.IsEmpty())
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Phrase Tree || Provided Transcription Segment is Empty ||"));

		return FParseResult(PHRASE_NOT_PARSED);
	}

	// First give the last visited node a chance to parse the phrase.
	// due to the possibility of connecting phrases over different transcription segments.
	if (LastVistedNode != nullptr && LastVistedNode.IsValid())
	{
		TArray<FString> PhraseWordArrayCopy = TArray(InPhraseWordArray);

		FParseResult ParseResult = LastVistedNode->ParseChildren(PhraseWordArrayCopy, LastVistedParseRecord);
		switch (ParseResult.Result)
		{
			case PHRASE_UNABLE_TO_PARSE:
			{
				// Continue Normal Propagation from Root or Context.
				break;
			}

			case PHRASE_REQUIRES_MORE:
			{
				// Return Due to Success Propagation, but keep last visited node for future attempts.
				InPhraseWordArray = PhraseWordArrayCopy;
				InParseRecord = LastVistedParseRecord;

				return ParseResult;
			}

			case PHRASE_PARSED:
			case PHRASE_PARSED_AND_EXECUTED:
			{
				InPhraseWordArray = PhraseWordArrayCopy;
				InParseRecord = LastVistedParseRecord;

				LastVistedNode.Reset();
				LastVistedParseRecord = FParseRecord();

				return ParseResult;
			}

			default:
			{
				return ParseResult;
			}
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

void FPhraseTree::BindBranch(const TPhraseNode &InNode)
{
	TArray<FPhraseTreeBranchBind> ToBindArray = TArray<FPhraseTreeBranchBind>();

	ToBindArray.Add(FPhraseTreeBranchBind(AsShared(), InNode));

	while (!ToBindArray.IsEmpty())
	{
		FPhraseTreeBranchBind BranchToBind = ToBindArray.Pop();

		// Check all ChildNodes to see if they are similar in purpose.
		for (auto &ChildNode : BranchToBind.StartNode->ChildNodes)
		{
			// If a ChildNode meets the same requirements as the BranchRoot,
			// then Split Bind Process to the ChildNodes.
			if (ChildNode->RequiresPhrase(BranchToBind.BranchRoot->BoundPhrase))
			{
				for (auto &BranchChildNode : BranchToBind.BranchRoot->ChildNodes)
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

void FPhraseTree::BindBranches(const TPhraseNodeArray &InNodes)
{
	for (const TSharedPtr<FPhraseNode> &Node : InNodes)
	{
		BindBranch(Node);
	}
}