#include "PhraseEvents/ViewportInteractionLibrary.h"
#include "PhraseEvents/Utils.h"

#include "Viewport/ViewportNavigator.h"

#include "EditorViewportClient.h"
#include <PhraseTree/Containers/Input/InputContainers.h>

#include "PhraseTree/PhraseNode.h"
#include "PhraseTree/PhraseIntInputNode.h"
#include "PhraseTree/PhraseEnumInputNode.h"
#include "PhraseTree/PhraseDirectionalInputNode.h"
#include "PhraseTree/PhraseEventNode.h"

namespace
{
	TSharedPtr<FAccessionViewportNavigator> GetViewportNavigator()
	{
		return FAccessionModule::Get().ViewportNavigator;
	}
}

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

				MakeShared<FPhraseNode>(TEXT("FOCUS"), 
					TPhraseNodeArray {
						MakeShared<FPhraseIntInputNode>(TEXT("INDEX"),
							TPhraseNodeArray{
								MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewportInteractionLibrary::FocusIndex))
						})
					}),


				MakeShared<FPhraseNode>(TEXT("NEXT"),
					TPhraseNodeArray{
						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewportInteractionLibrary::FocusNext))
					}),

				MakeShared<FPhraseNode>(TEXT("PREVIOUS"),
					TPhraseNodeArray{
						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewportInteractionLibrary::FocusPrevious))
					}),

				MakeShared<FPhraseNode>(TEXT("BACK"),
					TPhraseNodeArray{
						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewportInteractionLibrary::CameraBack))
					}),

				MakeShared<FPhraseNode>(TEXT("FRAME"),
					TPhraseNodeArray{
						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewportInteractionLibrary::FrameSelection))
					}),

				MakeShared<FPhraseNode>(TEXT("ORBIT"),
					TPhraseNodeArray{
						MakeShared<FPhrase2DDirectionalInputNode>(TEXT("DIRECTION"),
							TPhraseNodeArray{
								MakeShared<FPhraseIntInputNode>(TEXT("AMOUNT"),
									TPhraseNodeArray{
										MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewportInteractionLibrary::OrbitViewport))
									})
							})
					}),

				MakeShared<FPhraseNode>(TEXT("NUMBERS"),
					TPhraseNodeArray{
						MakeShared<FPhraseEventNode>(CreateParseDelegate(this, &UViewportInteractionLibrary::ToggleNumbers))
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

	FEditorViewportClient* ViewportClient = FAccessionViewportNavigator::ResolveActiveViewportClient();
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

	// A manual move takes over from any running focus / orbit animation.
	if (TSharedPtr<FAccessionViewportNavigator> Navigator = GetViewportNavigator())
	{
		Navigator->CancelTransition(ViewportClient);
	}

	PanCamera(ViewportClient, Direction, Amount);
};

void UViewportInteractionLibrary::FocusIndex(FParseRecord& Record)
{
	UParseIntInput* FocusInput = Record.GetPhraseInput<UParseIntInput>(TEXT("INDEX"));

	if (!FocusInput)
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("MoveViewport: Cannot Find Direction or Amount Input."))
			return;
	}

	// ----

	int32 FocusIndex = FocusInput->GetValue();

	if (TSharedPtr<FAccessionViewportNavigator> Navigator = GetViewportNavigator())
	{
		Navigator->FocusIndex(FocusIndex);
	}
}

void UViewportInteractionLibrary::FocusNext(FParseRecord& Record)
{
	if (TSharedPtr<FAccessionViewportNavigator> Navigator = GetViewportNavigator())
	{
		Navigator->FocusStep(+1);
	}
}

void UViewportInteractionLibrary::FocusPrevious(FParseRecord& Record)
{
	if (TSharedPtr<FAccessionViewportNavigator> Navigator = GetViewportNavigator())
	{
		Navigator->FocusStep(-1);
	}
}

void UViewportInteractionLibrary::CameraBack(FParseRecord& Record)
{
	if (TSharedPtr<FAccessionViewportNavigator> Navigator = GetViewportNavigator())
	{
		Navigator->Back();
	}
}

void UViewportInteractionLibrary::FrameSelection(FParseRecord& Record)
{
	if (TSharedPtr<FAccessionViewportNavigator> Navigator = GetViewportNavigator())
	{
		Navigator->FrameSelection();
	}
}

void UViewportInteractionLibrary::OrbitViewport(FParseRecord& Record)
{
	UParseEnumInput* DirectionInput = Record.GetPhraseInput<UParseEnumInput>(TEXT("DIRECTION"));
	UParseIntInput* AmountInput = Record.GetPhraseInput<UParseIntInput>(TEXT("AMOUNT"));

	if (!DirectionInput || !AmountInput)
	{
		UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("OrbitViewport: Cannot Find Direction or Amount Input."))
		return;
	}

	const float Degrees = FMath::Clamp<float>(AmountInput->GetValue(), 1.f, 180.f);

	float Yaw = 0.f, Pitch = 0.f;
	switch (EPhrase2DDirectionalInput(DirectionInput->GetValue()))
	{
		case EPhrase2DDirectionalInput::LEFT:	Yaw = Degrees; break;
		case EPhrase2DDirectionalInput::RIGHT:	Yaw = -Degrees; break;
		case EPhrase2DDirectionalInput::UP:		Pitch = Degrees; break;
		case EPhrase2DDirectionalInput::DOWN:	Pitch = -Degrees; break;

		default:
			UE_LOG(LogAccessionPhraseEvent, Warning, TEXT("OrbitViewport: Invalid Direction Input."))
			return;
	}

	if (TSharedPtr<FAccessionViewportNavigator> Navigator = GetViewportNavigator())
	{
		Navigator->Orbit(Yaw, Pitch);
	}
}

void UViewportInteractionLibrary::ToggleNumbers(FParseRecord& Record)
{
	if (TSharedPtr<FAccessionViewportNavigator> Navigator = GetViewportNavigator())
	{
		Navigator->ToggleLabels();
	}
}

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
		Verbose,
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

	// Keep the look-at point travelling with the camera, so ORBIT and orbit-mode viewports pivot on what is in view.
	ViewportClient->SetLookAtLocation(ViewportClient->GetLookAtLocation() + Delta, false);

	ViewportClient->Invalidate();
}
