#include "DrgHUD.h"
#include "DrgDamageWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Drg/UI/GameOver/GameOverWidget.h"
#include "Drg/UI/GameOver/GameResultData.h"
#include "Drg/GameModes/DrgPlayerState.h"
#include "Drg/System/DrgGameplayTags.h"

ADrgHUD::ADrgHUD()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = SceneComponent;
}

void ADrgHUD::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame(false);
	InitializeDamagePool();
	ShowInGameHUD();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	GameStateListenerHandle = MessageSubsystem.RegisterListener(
		DrgGameplayTags::Event_Broadcast_StateChanged,
		this,
		&ADrgHUD::OnGameStateChanged
	);
}

void ADrgHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GameStateListenerHandle.IsValid())
	{
		UGameplayMessageSubsystem::Get(GetWorld()).UnregisterListener(GameStateListenerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void ADrgHUD::OnGameStateChanged(FGameplayTag Channel, const FDrgGameStateChangeMessage& Message)
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	if (InGameHUDWidget)
	{
		InGameHUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	switch (Message.NewState)
	{
	case EGameFlowState::InGame:
		if (InGameHUDWidget) InGameHUDWidget->SetVisibility(ESlateVisibility::Visible);
		break;
	case EGameFlowState::Pause:
		ShowPauseMenu();
		break;
	case EGameFlowState::PostGame:
		{
			const bool bIsVictory = (Message.GameResult == EGameResult::Victory);
			ShowGameOverUI(bIsVictory);
			break;
		}
	default:
		// MainMenu 등 다른 상태에서는 아무것도 띄우지 않음
		break;
	}
}

void ADrgHUD::ShowGameOverUI(bool bIsVictory)
{
	if (!GameOverWidgetClass) return;

	UGameOverWidget* GameOverWidget = CreateWidget<UGameOverWidget>(GetOwningPlayerController(), GameOverWidgetClass);
	if (GameOverWidget)
	{
		if (ADrgPlayerState* PlayerState = GetOwningPlayerController()->GetPlayerState<ADrgPlayerState>())
		{
			const FGameResultData ResultData = PlayerState->GetGameResultData();
			GameOverWidget->SetGameResult(ResultData, bIsVictory);
		}
		GameOverWidget->AddToViewport();
		CurrentWidget = GameOverWidget;
	}
}

void ADrgHUD::ShowPauseMenu()
{
	if (!PauseMenuWidgetClass) return;

	CurrentWidget = CreateWidget(GetOwningPlayerController(), PauseMenuWidgetClass);
	if (CurrentWidget) CurrentWidget->AddToViewport();
}

void ADrgHUD::ShowInGameHUD()
{
	if (!InGameHUDWidgetClass) return;

	if (!InGameHUDWidget) InGameHUDWidget = CreateWidget(GetOwningPlayerController(), InGameHUDWidgetClass);

	if (InGameHUDWidget)
	{
		InGameHUDWidget->AddToViewport();
		InGameHUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ADrgHUD::InitializeDamagePool()
{
	if (DamageWidgetClass)
	{
		for (int32 i = 0; i < DamagePoolSize; ++i)
		{
			if (UWidgetComponent* NewWidgetComponent = NewObject<UWidgetComponent>(this))
			{
				NewWidgetComponent->RegisterComponent();
				NewWidgetComponent->AttachToComponent(
					GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				NewWidgetComponent->SetWidgetClass(DamageWidgetClass);
				NewWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
				NewWidgetComponent->SetDrawAtDesiredSize(true);
				NewWidgetComponent->SetVisibility(false);

				InactiveDamagePool.Add(NewWidgetComponent);
			}
		}
	}
}

void ADrgHUD::RequestDamageNumber(float Damage, FVector WorldLocation)
{
	UWidgetComponent* ComponentToUse = nullptr;
	if (InactiveDamagePool.Num() > 0)
	{
		ComponentToUse = InactiveDamagePool.Pop();
	}
	else if (ActiveDamagePool.Num() > 0)
	{
		ComponentToUse = ActiveDamagePool[0];
		ActiveDamagePool.RemoveAt(0);
	}

	if (ComponentToUse)
	{
		ComponentToUse->SetWorldLocation(WorldLocation);
		ComponentToUse->SetVisibility(true);

		if (UDrgDamageWidget* DamageWidget = Cast<UDrgDamageWidget>(ComponentToUse->GetUserWidgetObject()))
		{
			DamageWidget->SetDamageText(Damage);
			if (UWidgetAnimation* Anim = DamageWidget->GetFadeAndRiseAnimation())
			{
				DamageWidget->PlayAnimation(Anim);

				FTimerHandle ReturnTimer;
				FTimerDelegate ReturnDelegate = FTimerDelegate::CreateUObject(
					this, &ADrgHUD::ReturnToPool, ComponentToUse);
				GetWorld()->GetTimerManager().SetTimer(ReturnTimer, ReturnDelegate, Anim->GetEndTime(), false);
			}
		}
		ActiveDamagePool.Add(ComponentToUse);
	}
}

void ADrgHUD::ReturnToPool(UWidgetComponent* ComponentToReturn)
{
	if (ComponentToReturn)
	{
		ComponentToReturn->SetVisibility(false);
		ActiveDamagePool.Remove(ComponentToReturn);
		InactiveDamagePool.Add(ComponentToReturn);
	}
}
