#include "PhraseEvents/ViewInteractionLibrary.h"

#include "PhraseTree/Containers/Input/InputContainers.h"

#include "AssetAccessibilityRegistry.h"

void UViewInteractionLibrary::MoveViewport(FParseRecord &Record) 
{
	GET_ACTIVE_TAB(ActiveTab, SWidget)

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
    GET_ACTIVE_TAB(ActiveTab, SWidget)

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

        switch (EPhrase2DDirectionalInput(DirectionInput->GetValue())) {
            case EPhrase2DDirectionalInput::UP:
                ZoomAmount += AmountInput->GetValue();
                break;

            case EPhrase2DDirectionalInput::DOWN:
                ZoomAmount -= AmountInput->GetValue();
                break;

            default:
                UE_LOG(LogOpenAccessibilityPhraseEvent, Display, TEXT("ZoomViewport: INVALID DIRECTION INPUT"));
                return;
        }

        GraphEditor->SetViewLocation(ViewLocation, ZoomAmount);
    }

	// Further Viewport Specific Implementation Here
}

void UViewInteractionLibrary::IndexFocus(FParseRecord& Record) 
{
    GET_ACTIVE_TAB(ActiveTab, SWidget)

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
