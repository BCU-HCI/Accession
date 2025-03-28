#include "PhraseEvents/ViewInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "PhraseTree/Containers/Input/InputContainers.h"

#include "AccessionAssetRegistry.h"
#include "SGraphPanel.h"
#include "SNodePanel.h"

#include "PhraseTree/PhraseIntInputNode.h"
#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/PhraseEventNode.h"

UViewInteractionLibrary::UViewInteractionLibrary(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UViewInteractionLibrary::~UViewInteractionLibrary()
{
}

void UViewInteractionLibrary::BindBranches(TSharedRef<FPhraseTree> PhraseTree)
{
	PhraseTree->BindBranch(
		MakeShared<FPhraseNode>(TEXT("VIEW"),
								TPhraseNodeArray{

									MakeShared<FPhraseNode>(TEXT("MOVE"),
															TPhraseNodeArray{

																MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
																										  TPhraseNodeArray{

																											  MakeShared<FPhraseIntInputNode>(TEXT("AMOUNT"),
																																			  TPhraseNodeArray{

																																				  MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewInteractionLibrary::MoveViewport))

																																			  })

																										  })

															}),

									MakeShared<FPhraseNode>(TEXT("ZOOM"),
															TPhraseNodeArray{

																MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
																										  TPhraseNodeArray{

																											  MakeShared<FPhraseIntInputNode>(TEXT("AMOUNT"),
																																			  TPhraseNodeArray{

																																				  MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewInteractionLibrary::ZoomViewport))

																																			  })

																										  })

															}),

									MakeShared<FPhraseNode>(TEXT("FOCUS"),
															TPhraseNodeArray{

																MakeShared<FPhraseIntInputNode>(TEXT("INDEX"),
																								TPhraseNodeArray{

																									MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewInteractionLibrary::IndexFocus))

																								})

															})

								}));
}

void UViewInteractionLibrary::MoveViewport(FParseRecord &Record)
{
	GET_ACTIVE_TAB_CONTENT(ActiveTab)

	FString TabType = ActiveTab->GetTypeAsString();

	UParseEnumInput *DirectionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
	UParseIntInput *AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
	if (DirectionInput == nullptr || AmountInput == nullptr)
		return;

	if (TabType == "SGraphEditor")
	{
		TSharedPtr<SGraphEditor> GraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab);

		FVector2D ViewLocation;
		float ZoomAmount;
		GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);

		switch (EPhrase2DDirectionalInput(DirectionInput->GetValue()))
		{
		case EPhrase2DDirectionalInput::UP:
			ViewLocation.Y -= AmountInput->GetValue();
			break;

		case EPhrase2DDirectionalInput::DOWN:
			ViewLocation.Y += AmountInput->GetValue();
			break;

		case EPhrase2DDirectionalInput::LEFT:
			ViewLocation.X -= AmountInput->GetValue();
			break;

		case EPhrase2DDirectionalInput::RIGHT:
			ViewLocation.X += AmountInput->GetValue();
			break;

		default:
			UE_LOG(LogAccessionPhraseEvent, Display, TEXT("MoveViewport: INVALID DIRECTION INPUT"));
			return;
		}

		GraphEditor->SetViewLocation(ViewLocation, ZoomAmount);
	}

	// Further Viewport Implementation Here
}

class SOpenGraphPanel : public SGraphPanel
{
public:
	FZoomLevelsContainer *GetZoomLevels()
	{
		return ZoomLevels.Get();
	}
};

void UViewInteractionLibrary::ZoomViewport(FParseRecord &Record)
{
	GET_ACTIVE_TAB_CONTENT(ActiveTab)

	FString TabType = ActiveTab->GetTypeAsString();

	UParseEnumInput *DirectionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
	UParseIntInput *AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
	if (DirectionInput == nullptr || AmountInput == nullptr)
		return;

	if (TabType == "SGraphEditor")
	{
		TSharedPtr<SGraphEditor> GraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab);
		FZoomLevelsContainer *ZoomLevels;

		// Another Hack because its protected, which is good.
		// But the derived ZoomLevelsContainer is in a .cpp file :(
		{
			SOpenGraphPanel *GraphPanel = static_cast<SOpenGraphPanel *>(GraphEditor->GetGraphPanel());
			ZoomLevels = GraphPanel->GetZoomLevels();

			if (ZoomLevels == nullptr)
				return;
		}

		FVector2D ViewLocation;
		float ZoomAmount;
		GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);

		// Find Index for Current Zoom Level
		int32 ZoomIndex;
		for (ZoomIndex = 0; ZoomIndex < ZoomLevels->GetNumZoomLevels(); ZoomIndex++)
		{
			if (ZoomAmount <= ZoomLevels->GetZoomAmount(ZoomIndex))
			{
				break;
			}
		}

		switch (EPhrase2DDirectionalInput(DirectionInput->GetValue()))
		{
		case EPhrase2DDirectionalInput::UP:
			ZoomIndex += AmountInput->GetValue();
			break;

		case EPhrase2DDirectionalInput::DOWN:
			ZoomIndex -= AmountInput->GetValue();
			break;

		default:
			UE_LOG(LogAccessionPhraseEvent, Display, TEXT("ZoomViewport: INVALID DIRECTION INPUT"));
			return;
		}

		if (ZoomIndex < 0 || ZoomIndex >= ZoomLevels->GetNumZoomLevels())
			ZoomAmount = ZoomLevels->GetDefaultZoomLevel();
		else
			ZoomAmount = ZoomLevels->GetZoomAmount(ZoomIndex);

		GraphEditor->SetViewLocation(ViewLocation, ZoomAmount);
	}

	// Further Viewport Specific Implementation Here
}

void UViewInteractionLibrary::IndexFocus(FParseRecord &Record)
{
	GET_ACTIVE_TAB_CONTENT(ActiveTab)

	FString TabType = ActiveTab->GetTypeAsString();

	UParseIntInput *IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("INDEX"));
	if (IndexInput == nullptr)
		return;

	if (TabType == "SGraphEditor")
	{
		TSharedPtr<SGraphEditor> GraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab);
		if (!GraphEditor.IsValid())
			return;

		TSharedRef<FAccessionAssetRegistry> AssetRegistry = GetAssetRegistry();

		TSharedRef<FGraphIndexer> GraphIndexer = AssetRegistry->GetGraphIndexer(GraphEditor->GetCurrentGraph());

		UEdGraphNode *Node = GraphIndexer->GetNode(IndexInput->GetValue());
		if (Node == nullptr)
		{
			UE_LOG(LogAccessionPhraseEvent, Display, TEXT("IndexFocus: INVALID INDEX INPUT"))
			return;
		}

		GraphEditor->JumpToNode(Node);
	}

	// Further ViewportS Specific Implementation Here
}
