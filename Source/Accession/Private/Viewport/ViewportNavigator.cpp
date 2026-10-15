// Copyright (C) HCI-BCU 2025. All rights reserved.

#include "Viewport/ViewportNavigator.h"
#include "Viewport/AccessionViewportSettings.h"
#include "AccessionLogging.h"

#include "Editor.h"
#include "EditorViewportClient.h"
#include "LevelEditorViewport.h"
#include "SEditorViewport.h"
#include "Engine/Selection.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "SceneView.h"
#include "Debug/DebugDrawService.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"

namespace ViewportNavigatorConstants
{
	/** How often the active viewport is re-resolved from Slate focus. */
	static constexpr float ResolveInterval = 0.2f;

	/** How often a still camera re-indexes, to pick up actors that were added, moved or deleted. */
	static constexpr double IdleRefreshInterval = 1.0;

	/** Colours shared with SIndexer on graph nodes, so 2D and 3D labels read the same. */
	static const FLinearColor LabelBackground(0.f, 0.f, 0.f, 0.8f);
	static const FLinearColor LabelBorder = FLinearColor::Gray;
	static const FLinearColor LabelFocusedBorder(1.f, 0.75f, 0.f, 1.f);
	static const FLinearColor LabelText = FLinearColor::White;
}

// Lifetime

void FAccessionViewportNavigator::Initialize()
{
	bLabelsVisible = GetDefault<UAccessionViewportSettings>()->bShowLabels;

	TickHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateRaw(this, &FAccessionViewportNavigator::Tick));

	// "Editor" draws on every editor viewport's debug canvas; DrawLabels filters to the active one.
	DrawHandle = UDebugDrawService::Register(
		TEXT("Editor"),
		FDebugDrawDelegate::CreateRaw(this, &FAccessionViewportNavigator::DrawLabels));
}

void FAccessionViewportNavigator::Shutdown()
{
	if (TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
		TickHandle.Reset();
	}

	if (DrawHandle.IsValid())
	{
		UDebugDrawService::Unregister(DrawHandle);
		DrawHandle.Reset();
	}

	States.Reset();
	LabelledClient = nullptr;
}

// Viewport Resolution

bool FAccessionViewportNavigator::IsClientAlive(const FEditorViewportClient *Client)
{
	return Client && GEditor && GEditor->GetAllViewportClients().Contains(const_cast<FEditorViewportClient *>(Client));
}

FEditorViewportClient *FAccessionViewportNavigator::ResolveActiveViewportClient()
{
	if (!GEditor || !FSlateApplication::IsInitialized())
		return nullptr;

	TSharedPtr<SWidget> ActiveContent;
	if (TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab())
	{
		ActiveContent = ActiveTab->GetContent();
	}

	// Graph editors have their own FOCUS / VIEW handling, never move a hidden 3D camera from one.
	if (ActiveContent.IsValid() && ActiveContent->GetType() == TEXT("SGraphEditor"))
		return nullptr;

	// Every viewport inside the active tab (several in a multi-pane layout).
	TArray<FEditorViewportClient *> Candidates;
	if (ActiveContent.IsValid())
	{
		for (FEditorViewportClient *Client : GEditor->GetAllViewportClients())
		{
			if (!Client)
				continue;

			const TSharedPtr<SEditorViewport> EditorViewport = Client->GetEditorViewportWidget();
			if (!EditorViewport.IsValid())
				continue;

			if (static_cast<SWidget *>(EditorViewport.Get()) == ActiveContent.Get())
			{
				Candidates.Add(Client);
				continue;
			}

			FWidgetPath Path;
			if (FSlateApplication::Get().FindPathToWidget(EditorViewport.ToSharedRef(), Path, EVisibility::Visible) &&
				Path.ContainsWidget(ActiveContent.Get()))
			{
				Candidates.Add(Client);
			}
		}
	}

	FEditorViewportClient *LastLevelClient = IsClientAlive(GCurrentLevelEditingViewportClient)
												 ? GCurrentLevelEditingViewportClient
												 : nullptr;

	if (Candidates.Num() == 1)
		return Candidates[0];

	if (Candidates.Num() > 1)
	{
		// Prefer the pane the user last interacted with.
		const FViewport *ActiveViewport = GEditor->GetActiveViewport();
		for (FEditorViewportClient *Client : Candidates)
		{
			if (Client->Viewport == ActiveViewport)
				return Client;
		}

		if (LastLevelClient && Candidates.Contains(LastLevelClient))
			return LastLevelClient;

		return Candidates[0];
	}

	// The active tab has no viewport (e.g. Details or Outliner has focus): act on the last used level viewport.
	return LastLevelClient;
}

FAccessionViewportNavigator::FViewportState &FAccessionViewportNavigator::GetOrAddState(FEditorViewportClient *Client)
{
	if (TUniquePtr<FViewportState> *Existing = States.Find(Client))
		return **Existing;

	return *States.Add(Client, MakeUnique<FViewportState>());
}

FEditorViewportClient *FAccessionViewportNavigator::GetCommandClient(FViewportState *&OutState)
{
	OutState = nullptr;

	FEditorViewportClient *Client = ResolveActiveViewportClient();
	if (!Client)
	{
		UE_LOG(LogAccession, Display, TEXT("ViewportNavigator: No Active Viewport For Command."));
		return nullptr;
	}

	// Commands act where the user is looking, move the labels there too.
	SetLabelledClient(Client);

	OutState = &GetOrAddState(Client);

	if (OutState->Indexer.Num() == 0)
	{
		OutState->Indexer.Refresh(Client);
		OutState->bNeedsRefresh = false;
		OutState->LastRefreshTime = FPlatformTime::Seconds();
	}

	return Client;
}

// Commands

bool FAccessionViewportNavigator::FocusIndex(int32 Index)
{
	FViewportState *State;
	FEditorViewportClient *Client = GetCommandClient(State);
	if (!Client)
		return false;

	const FViewportTarget *Target = State->Indexer.Find(Index);
	if (!Target)
	{
		UE_LOG(LogAccession, Display, TEXT("ViewportNavigator: No Item Numbered %d."), Index);
		return false;
	}

	UE_LOG(LogAccession, Log, TEXT("ViewportNavigator: Focusing %d (%s)."), Index, *Target->GetLabel());

	State->Camera.FocusOnBox(Client, Target->GetBounds());
	State->FocusedIndex = Index;
	Client->Invalidate();

	return true;
}

bool FAccessionViewportNavigator::FocusStep(int32 Step)
{
	FViewportState *State;
	FEditorViewportClient *Client = GetCommandClient(State);
	if (!Client)
		return false;

	const TArray<int32> Indexes = State->Indexer.GetSortedIndexes();
	if (Indexes.IsEmpty())
	{
		UE_LOG(LogAccession, Display, TEXT("ViewportNavigator: No Items To Step Through."));
		return false;
	}

	int32 Position = Indexes.Find(State->FocusedIndex);
	if (Position == INDEX_NONE)
	{
		// Nothing focused yet (or it left the view): NEXT starts at the first item, PREVIOUS at the last.
		Position = Step > 0 ? -1 : 0;
	}

	const int32 NextPosition = ((Position + Step) % Indexes.Num() + Indexes.Num()) % Indexes.Num();

	return FocusIndex(Indexes[NextPosition]);
}

bool FAccessionViewportNavigator::Back()
{
	FViewportState *State;
	FEditorViewportClient *Client = GetCommandClient(State);
	if (!Client)
		return false;

	if (!State->Camera.Back(Client))
	{
		UE_LOG(LogAccession, Display, TEXT("ViewportNavigator: No Camera History."));
		return false;
	}

	State->FocusedIndex = INDEX_NONE;
	return true;
}

bool FAccessionViewportNavigator::Orbit(float YawDegrees, float PitchDegrees)
{
	FViewportState *State;
	FEditorViewportClient *Client = GetCommandClient(State);
	if (!Client)
		return false;

	State->Camera.Orbit(Client, YawDegrees, PitchDegrees);
	return true;
}

bool FAccessionViewportNavigator::FrameSelection()
{
	FViewportState *State;
	FEditorViewportClient *Client = GetCommandClient(State);
	if (!Client)
		return false;

	FBox Bounds(ForceInit);

	// The editor selection lives in the level world; only use it when this viewport shows that world.
	if (GEditor && Client->GetWorld() == GEditor->GetEditorWorldContext().World())
	{
		for (FSelectionIterator It(*GEditor->GetSelectedActors()); It; ++It)
		{
			if (AActor *Actor = Cast<AActor>(*It))
			{
				FViewportTarget Target;
				Target.Actor = Actor;
				Bounds += Target.GetBounds();
			}
		}
	}

	if (!Bounds.IsValid)
	{
		for (const TPair<int32, FViewportTarget> &Entry : State->Indexer.GetEntries())
		{
			if (Entry.Value.IsValid())
			{
				Bounds += Entry.Value.GetBounds();
			}
		}
	}

	if (!Bounds.IsValid)
	{
		UE_LOG(LogAccession, Display, TEXT("ViewportNavigator: Nothing To Frame."));
		return false;
	}

	State->Camera.FocusOnBox(Client, Bounds);
	State->FocusedIndex = INDEX_NONE;
	return true;
}

void FAccessionViewportNavigator::ToggleLabels()
{
	bLabelsVisible = !bLabelsVisible;

	if (IsClientAlive(LabelledClient))
	{
		LabelledClient->Invalidate();
	}
}

void FAccessionViewportNavigator::CancelTransition(FEditorViewportClient *Client)
{
	if (TUniquePtr<FViewportState> *State = States.Find(Client))
	{
		(*State)->Camera.CancelTransition();
	}
}

// Tick

bool FAccessionViewportNavigator::Tick(float DeltaTime)
{
	PruneDeadClients();

	ResolveAccumulator += DeltaTime;
	if (ResolveAccumulator >= ViewportNavigatorConstants::ResolveInterval)
	{
		ResolveAccumulator = 0.f;
		SetLabelledClient(ResolveActiveViewportClient());
	}

	for (TPair<FEditorViewportClient *, TUniquePtr<FViewportState>> &Pair : States)
	{
		Pair.Value->Camera.Tick(Pair.Key, DeltaTime);
	}

	if (LabelledClient)
	{
		UpdateIndexing(LabelledClient, GetOrAddState(LabelledClient));
	}

	return true;
}

void FAccessionViewportNavigator::UpdateIndexing(FEditorViewportClient *Client, FViewportState &State)
{
	const double Now = FPlatformTime::Seconds();
	const FViewportCameraPose Pose = FViewportCameraPose::Capture(Client);

	// Numbers are only re-assigned once the camera is still, so they never change while the user is reading them.
	if (!Pose.NearlyEquals(State.LastObservedPose))
	{
		State.LastObservedPose = Pose;
		State.LastMovedTime = Now;
		State.bNeedsRefresh = true;
		return;
	}

	if (State.Camera.IsTransitioning())
		return;

	const bool bSettled = (Now - State.LastMovedTime) >= GetDefault<UAccessionViewportSettings>()->SettleDelaySeconds;
	const bool bIdleRefreshDue = (Now - State.LastRefreshTime) >= ViewportNavigatorConstants::IdleRefreshInterval;

	if (bSettled && (State.bNeedsRefresh || bIdleRefreshDue))
	{
		State.Indexer.Refresh(Client);
		State.bNeedsRefresh = false;
		State.LastRefreshTime = Now;

		if (bLabelsVisible)
		{
			Client->Invalidate();
		}
	}
}

void FAccessionViewportNavigator::PruneDeadClients()
{
	for (auto It = States.CreateIterator(); It; ++It)
	{
		if (!IsClientAlive(It.Key()))
		{
			It.RemoveCurrent();
		}
	}

	if (LabelledClient && !IsClientAlive(LabelledClient))
	{
		LabelledClient = nullptr;
	}
}

void FAccessionViewportNavigator::SetLabelledClient(FEditorViewportClient *NewClient)
{
	if (NewClient == LabelledClient)
		return;

	// Redraw the old viewport so its labels disappear.
	if (IsClientAlive(LabelledClient))
	{
		LabelledClient->Invalidate();
	}

	LabelledClient = NewClient;

	if (LabelledClient)
	{
		GetOrAddState(LabelledClient).bNeedsRefresh = true;
		LabelledClient->Invalidate();
	}
}

// Labels

void FAccessionViewportNavigator::DrawLabels(UCanvas *Canvas, APlayerController *PlayerController)
{
	if (!bLabelsVisible || !Canvas || !Canvas->SceneView || !Canvas->SceneView->Family || !GEngine)
		return;

	if (!IsClientAlive(LabelledClient) ||
		Canvas->SceneView->Family->RenderTarget != static_cast<const FRenderTarget *>(LabelledClient->Viewport))
		return;

	const TUniquePtr<FViewportState> *StatePtr = States.Find(LabelledClient);
	if (!StatePtr)
		return;

	const FViewportState &State = **StatePtr;
	const FSceneView &View = *Canvas->SceneView;

	UFont *Font = GEngine->GetLargeFont();
	if (!Font)
		return;

	const float Scale = GetDefault<UAccessionViewportSettings>()->LabelScale;
	const float Padding = 4.f * Scale;
	const float TextHeight = Font->GetMaxCharHeight() * Scale;

	TArray<FBox2D> PlacedLabels;

	for (const int32 Index : State.Indexer.GetSortedIndexes())
	{
		const FViewportTarget *Target = State.Indexer.Find(Index);
		if (!Target)
			continue;

		const FBox Bounds = Target->GetBounds();
		if (!Bounds.IsValid)
			continue;

		// Project the item centre into canvas space (same maths as UCanvas::Project, without its behind-camera clamping).
		const FVector4 ScreenPoint = View.WorldToScreen(Bounds.GetCenter());
		if (ScreenPoint.W <= UE_KINDA_SMALL_NUMBER)
			continue;

		const FVector2D Ndc(ScreenPoint.X / ScreenPoint.W, ScreenPoint.Y / ScreenPoint.W);
		if (FMath::Abs(Ndc.X) > 1.0 || FMath::Abs(Ndc.Y) > 1.0)
			continue;

		const FVector2D Anchor(
			Canvas->OrgX + (Ndc.X * 0.5 + 0.5) * Canvas->ClipX,
			Canvas->OrgY + (-Ndc.Y * 0.5 + 0.5) * Canvas->ClipY);

		const FString Text = FString::FromInt(Index);
		const float TextWidth = Font->GetStringSize(*Text) * Scale;

		const FVector2D BoxSize(FMath::Max(TextWidth, TextHeight) + Padding * 2.f, TextHeight + Padding * 2.f);
		FVector2D BoxPos = Anchor - BoxSize * 0.5;

		// Nudge overlapping labels downwards so every number stays readable.
		for (int32 Attempt = 0; Attempt < 6; ++Attempt)
		{
			const FBox2D Candidate(BoxPos, BoxPos + BoxSize);
			const bool bOverlaps = PlacedLabels.ContainsByPredicate([&Candidate](const FBox2D &Placed)
																	{ return Placed.Intersect(Candidate); });
			if (!bOverlaps)
				break;

			BoxPos.Y += BoxSize.Y + 2.0;
		}
		PlacedLabels.Add(FBox2D(BoxPos, BoxPos + BoxSize));

		const bool bFocused = Index == State.FocusedIndex;

		FCanvasTileItem Background(BoxPos, BoxSize, ViewportNavigatorConstants::LabelBackground);
		Background.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Background);

		FCanvasBoxItem Border(BoxPos, BoxSize);
		Border.SetColor(bFocused ? ViewportNavigatorConstants::LabelFocusedBorder : ViewportNavigatorConstants::LabelBorder);
		Border.LineThickness = (bFocused ? 3.f : 1.5f) * Scale;
		Canvas->DrawItem(Border);

		FCanvasTextItem Label(BoxPos + BoxSize * 0.5, FText::FromString(Text), Font, ViewportNavigatorConstants::LabelText);
		Label.bCentreX = true;
		Label.bCentreY = true;
		Label.Scale = FVector2D(Scale, Scale);
		Canvas->DrawItem(Label);
	}
}
