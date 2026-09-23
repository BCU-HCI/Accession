#include "PhraseEvents/ViewportInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "Widgets/SViewport.h"
#include "SEditorViewport.h"
#include "Slate/SceneViewport.h"
#include <PhraseTree/Containers/Input/InputContainers.h>

void UViewportInteractionLibrary::BindBranches(TSharedRef<FPhraseTree> PhraseTree)
{

}

void UViewportInteractionLibrary::MoveViewport(FParseRecord& Record)
{
	GET_ACTIVE_TAB_CONTENT(ActiveTabContent);

	FEditorViewportClient* ViewportClient = GetViewportClient(ActiveTabContent);
	if (!ViewportClient)
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("MoveViewport: Cannot Find Viewport Client."))
			return;
	}

	UParseEnumInput* DirectionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
	UParseIntInput* AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));

	if (!DirectionInput || !AmountInput)
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("MoveViewport: Cannot Find Direction or Amount Input."))
			return;
	}

	FVector2D Direction;
	switch (EPhrase2DDirectionalInput(DirectionInput->GetValue()))
	{
		case EPhrase2DDirectionalInput::UP:
			Direction = FVector2D(0, 1);
			break;
		case EPhrase2DDirectionalInput::DOWN:
			Direction = FVector2D(0, -1);
			break;
		case EPhrase2DDirectionalInput::LEFT:
			Direction = FVector2D(-1, 0);
			break;
		case EPhrase2DDirectionalInput::RIGHT:
			Direction = FVector2D(1, 0);
			break;
		default:
			UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("MoveViewport: Invalid Direction Input."))
			return;
	}

	int32 Amount = AmountInput->GetValue();
	if (Amount <= 0)
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("MoveViewport: Amount Input Must Be Greater Than Zero."))
			return;
	}

	PanCamera(ViewportClient, Direction, Amount);
};

void UViewportInteractionLibrary::ZoomViewport(FParseRecord& Record)
{
	GET_ACTIVE_TAB_CONTENT(ActiveTabContent);

}

void UViewportInteractionLibrary::PanCamera(FEditorViewportClient* ViewportClient, const FVector2D& Direction, float Amount)
{
	if (!ViewportClient)
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("PanCamera: Invalid Viewport Client."))
		return;
	}

	const FRotationMatrix RotationMatrix(ViewportClient->GetViewRotation());

	const FVector Right = RotationMatrix.GetScaledAxis(EAxis::Y);
	const FVector Up = RotationMatrix.GetScaledAxis(EAxis::Z);

	const FVector PanDelta = (Right * Direction.X + Up * Direction.Y) * Amount;

	ViewportClient->SetViewLocation(ViewportClient->GetViewLocation() + PanDelta);
	ViewportClient->SetLookAtLocation(ViewportClient->GetLookAtLocation() + PanDelta, false);


	ViewportClient->Invalidate();
}

FEditorViewportClient* UViewportInteractionLibrary::GetViewportClient(const TSharedPtr<SWidget>& Widget)
{
	if (!Widget.IsValid() || !GEditor)
	{
		return nullptr;
	}

	SViewport* test;
	test->GetType();

	if (Widget->GetType() != SViewport::StaticWidgetClass().GetWidgetType())
	{
		return nullptr;
	}


	const TSharedPtr<SViewport> ViewportWidget = StaticCastSharedPtr<SViewport>(Widget);

	for (FEditorViewportClient* ViewportClient : GEditor->GetAllViewportClients())
	{
		if (!ViewportClient)
		{
			continue;
		}
		
		const TSharedPtr<SEditorViewport> EditorViewportWidget = ViewportClient->GetEditorViewportWidget();
		if (!EditorViewportWidget.IsValid())
		{
			continue;
		}

		const TSharedPtr<FSceneViewport> SceneViewport = EditorViewportWidget->GetSceneViewport();
		if (!SceneViewport.IsValid())
		{
			continue;
		}

		const TSharedPtr<SViewport> SceneViewportWidget = SceneViewport->GetViewportWidget().Pin();
		if (SceneViewportWidget == ViewportWidget)
		{
			return ViewportClient;
		}
	}

	return nullptr;
}