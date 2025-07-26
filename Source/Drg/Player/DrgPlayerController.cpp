// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Drg/AbilitySystem/Abilities/DrgGameplayAbility.h"

ADrgPlayerController::ADrgPlayerController()
{
}

void ADrgPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		check(DefaultMappingContext);
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ADrgPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		check(MoveAction);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADrgPlayerController::Move);
		check(AttackAction);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ADrgPlayerController::Attack);
	}
}

void ADrgPlayerController::Move(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D MovementVector = Value.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);

		if (MovementVector.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			ControlledPawn->AddMovementInput(MovementDirection, MovementVector.X);
		}

		if (MovementVector.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			ControlledPawn->AddMovementInput(MovementDirection, MovementVector.Y);
		}
	}
}

void ADrgPlayerController::Attack(const FInputActionValue& Value)
{
	if (AttackInputTag.IsValid())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), AttackInputTag, FGameplayEventData());
	}
}
