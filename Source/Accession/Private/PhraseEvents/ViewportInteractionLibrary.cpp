#include "PhraseEvents/ViewportInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "Widgets/SViewport.h"
#include "SEditorViewport.h"
#include "Slate/SceneViewport.h"
#include <PhraseTree/Containers/Input/InputContainers.h>

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/PhraseIntInputNode.h"
#include "PhraseTree/PhraseEnumInputNode.h"
#include "PhraseTree/PhraseEventNode.h"

void UViewportInteractionLibrary::BindBranches(TSharedRef<FPhraseTree> PhraseTree)
{
	PhraseTree->BindBranch(
		MakeShared<FPhraseNode>(TEXT("VIEW"), 
			TPhraseNodeArray{

				MakeShared<FPhraseNode>(TEXT("MOVE"),
					TPhraseNodeArray{

						MakeShared<FPhraseEnumInputNode<EPhraseDirectionalInput>>(TEXT("DIRECTION"),
							TPhraseNodeArray{
								MakeShared<FPhraseIntInputNode>(TEXT("AMOUNT"),
									TPhraseNodeArray{
										MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewportInteractionLibrary::MoveViewport))
									})
							})

					}),
			})
	);
	
}

void UViewportInteractionLibrary::MoveViewport(FParseRecord& Record)
{
	UParseEnumInput* DirectionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
	UParseIntInput* AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));

	if (!DirectionInput || !AmountInput)
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("MoveViewport: Cannot Find Direction or Amount Input."))
			return;
	}

	// ----

	GET_ACTIVE_TAB_CONTENT(ActiveTabContent);

	FEditorViewportClient* ViewportClient = GetViewportClient(ActiveTabContent);
	if (!ViewportClient)
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("MoveViewport: Cannot Find Viewport Client."))
			return;
	}

	FVector Direction;
	switch (EPhraseDirectionalInput(DirectionInput->GetValue()))
	{
		case EPhraseDirectionalInput::FORWARD:
			Direction = FVector::ForwardVector;
			break;

		case EPhraseDirectionalInput::BACKWARD:
			Direction = FVector::BackwardVector;
			break;

		case EPhraseDirectionalInput::UP:
			Direction = FVector::UpVector;
			break;

		case EPhraseDirectionalInput::DOWN:
			Direction = FVector::DownVector;
			break;

		case EPhraseDirectionalInput::LEFT:
			Direction = FVector::LeftVector;
			break;

		case EPhraseDirectionalInput::RIGHT:
			Direction = FVector::RightVector;
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


	UE_LOG(LogAccessionPhraseEvent, Log, TEXT("Active Tab Content: %s"), *ActiveTabContent->GetType().ToString());

	PanCamera(ViewportClient, Direction, Amount);
};

void UViewportInteractionLibrary::PanCamera(FEditorViewportClient* ViewportClient, const FVector& Direction, float Amount)
{
	if (!ViewportClient)
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("PanCamera: Invalid Viewport Client."))
		return;
	}
	
	const FVector InitialView = ViewportClient->GetViewLocation();
	const FRotator ClientRotator = ViewportClient->GetViewRotation();

	const FVector WorldDirection = ClientRotator.RotateVector(Direction);
	const FVector Delta = WorldDirection * Amount;

	UE_LOG(
		LogAccessionPhraseEvent,
		Warning,
		TEXT(
			"PAN Client=%p Orbit=%d Ortho=%d "
			"Before=%s Delta=%s"),
		ViewportClient,
		ViewportClient->ShouldOrbitCamera(),
		ViewportClient->IsOrtho(),
		*InitialView.ToString(),
		*Delta.ToString()
	);

	ViewportClient->SetViewLocation(InitialView + Delta);

	if (ViewportClient->ShouldOrbitCamera())
	{
		ViewportClient->SetLookAtLocation(ViewportClient->GetLookAtLocation() + Delta, false);
	}

	UE_LOG(
		LogAccessionPhraseEvent,
		Warning,
		TEXT("After=%s"),
		*ViewportClient->GetViewLocation().ToString()
	);

	ViewportClient->Invalidate();
}

FEditorViewportClient*
UViewportInteractionLibrary::GetViewportClient(
	const TSharedPtr<SWidget>& Widget)
{
	if (!Widget.IsValid() ||
		!GEditor ||
		!FSlateApplication::IsInitialized())
	{
		return nullptr;
	}

	FEditorViewportClient* FoundClient = nullptr;

	for (FEditorViewportClient* ViewportClient :
		GEditor->GetAllViewportClients())
	{
		if (!ViewportClient)
		{
			continue;
		}

		const TSharedPtr<SEditorViewport> EditorViewport =
			ViewportClient->GetEditorViewportWidget();

		if (!EditorViewport.IsValid())
		{
			continue;
		}

		// Active Content is the Viewport
		if (static_cast<SWidget*>(EditorViewport.Get()) ==
			Widget.Get())
		{
			return ViewportClient;
		}

		// Otherwise Validate Widget Path if Descendant Client.
		FWidgetPath Path;

		if (!FSlateApplication::Get().FindPathToWidget(
			EditorViewport.ToSharedRef(),
			Path,
			EVisibility::Visible))
		{
			continue;
		}

		if (Path.ContainsWidget(Widget.Get()))
		{
			// Active widget contains this viewport.
			FoundClient = ViewportClient;
		}
	}

	return FoundClient;
}