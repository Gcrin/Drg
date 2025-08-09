// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgGameStateBase.h"

#include "DrgGameModeBase.h"
#include "Drg/System/DrgGameplayTags.h"
#include "Net/UnrealNetwork.h"

void ADrgGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADrgGameStateBase, CurrentGameResult);
}

void ADrgGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		DeathMessageListenerHandle = MessageSubsystem.RegisterListener(
			DrgGameplayTags::Event_Death,
			this,
			&ADrgGameStateBase::OnDeathMessageReceived
		);
	}
}

void ADrgGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DeathMessageListenerHandle.IsValid())
	{
		UGameplayMessageSubsystem::Get(GetWorld()).UnregisterListener(DeathMessageListenerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void ADrgGameStateBase::OnDeathMessageReceived(FGameplayTag Channel, const FDrgDeathMessage& Message)
{
	ADrgGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ADrgGameModeBase>();
	check(GameMode != nullptr);

	const EGameResult Result = GameMode->EvaluateGameEndCondition(Message.DeadActor);

	if (Result != EGameResult::None)
	{
		CurrentGameResult = Result;

		if (HasAuthority())
		{
			OnRep_GameResult();
		}
	}
}

void ADrgGameStateBase::OnRep_GameResult()
{
	if (CurrentGameResult != EGameResult::None)
	{
		UGameInstance* GameInstance = GetGameInstance();
		check(GameInstance != nullptr);

		UDrgGameStateManagerSubsystem* GameStateManager = GameInstance->GetSubsystem<UDrgGameStateManagerSubsystem>();
		check(GameStateManager != nullptr);

		GameStateManager->ChangeStateWithResult(EGameFlowState::PostGame, CurrentGameResult);
	}
}
