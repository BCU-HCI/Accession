#include "PhraseEvents/ViewInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "PhraseTree/Containers/Input/InputContainers.h"

#include "AssetAccessibilityRegistry.h"

#include "PhraseTree/PhraseInputNode.h"
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
		TPhraseNodeArray {
			
			MakeShared<FPhraseNode>(TEXT("MOVE"),
			TPhraseNodeArray {

				MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"), 
				TPhraseNodeArray {
				
					MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"),
					TPhraseNodeArray {
					
						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewInteractionLibrary::MoveViewport))

					})

				})

			}),

			MakeShared<FPhraseNode>(TEXT("ZOOM"), 
			TPhraseNodeArray {
			
				MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"), 
				TPhraseNodeArray {
				
					MakeShared<FPhraseInputNode<int32>>(TEXT("AMOUNT"), 
					TPhraseNodeArray {
					
						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewInteractionLibrary::ZoomViewport))
					
					})

				})
			
			}),

			MakeShared<FPhraseNode>(TEXT("FOCUS"), 
			TPhraseNodeArray {
				
				MakeShared<FPhraseInputNode<int32>>(TEXT("INDEX"), 
				TPhraseNodeArray {
				
					MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewInteractionLibrary::IndexFocus))

				})

			})

		})
	);
}

void UViewInteractionLibrary::MoveViewport(FParseRecord &Record) {
	GET_ACTIVE_TAB_CONTENT(ActiveTab)

	FString TabType = ActiveTab->GetTypeAsString();

	UParseEnumInput* DirectionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
    UParseIntInput* AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
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
				UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("MoveViewport: INVALID DIRECTION INPUT"));
				return;
		}

		GraphEditor->SetViewLocation(ViewLocation, ZoomAmount);
	}

	// Further Viewport Implementation Here
}

void UViewInteractionLibrary::ZoomViewport(FParseRecord &Record) 
{
	GET_ACTIVE_TAB_CONTENT(ActiveTab)

    FString TabType = ActiveTab->GetTypeAsString();

    UParseEnumInput* DirectionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
    UParseIntInput* AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));
    if (DirectionInput == nullptr || AmountInput == nullptr)
        return;

    if (TabType == "SGraphEditor")
	{
		#define ZOOM_INCREMENT 0.125f

        TSharedPtr<SGraphEditor> GraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab);

        FVector2D ViewLocation;
        float ZoomAmount;
        GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);

		ZoomAmount = FMath::RoundToFloat(ZoomAmount / ZOOM_INCREMENT) * ZOOM_INCREMENT;

        switch (EPhrase2DDirectionalInput(DirectionInput->GetValue())) {
            case EPhrase2DDirectionalInput::UP:
                ZoomAmount += (ZOOM_INCREMENT * AmountInput->GetValue());
                break;

            case EPhrase2DDirectionalInput::DOWN:
                ZoomAmount -= (ZOOM_INCREMENT * AmountInput->GetValue());
                break;

            default:
                UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("ZoomViewport: INVALID DIRECTION INPUT"));
                return;
        }

        GraphEditor->SetViewLocation(ViewLocation, ZoomAmount);

		#undef ZOOM_INCREMENT
    }

	// Further Viewport Specific Implementation Here
}

void UViewInteractionLibrary::IndexFocus(FParseRecord& Record) 
{
	GET_ACTIVE_TAB_CONTENT(ActiveTab)

	FString TabType = ActiveTab->GetTypeAsString();

	UParseIntInput* IndexInput = Record.GetPhraseInput<UParseIntInput>(TEXT("INDEX"));
	if (IndexInput == nullptr)
		return;

	if (TabType == "SGraphEditor")
	{
		TSharedPtr<SGraphEditor> GraphEditor = StaticCastSharedPtr<SGraphEditor>(ActiveTab);
        if (!GraphEditor.IsValid())
			return;

		TSharedRef<FAssetAccessibilityRegistry> AssetRegistry = GetAssetRegistry();

		TSharedRef<FGraphIndexer> GraphIndexer = AssetRegistry->GetGraphIndexer(GraphEditor->GetCurrentGraph());

		UEdGraphNode* Node = GraphIndexer->GetNode(IndexInput->GetValue());
		if (Node == nullptr)
		{
			UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("IndexFocus: INVALID INDEX INPUT"))
			return;
		}

		GraphEditor->JumpToNode(Node);
	}

	// Further ViewportS Specific Implementation Here
}
