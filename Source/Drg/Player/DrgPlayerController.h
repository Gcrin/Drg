// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "DrgPlayerController.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class DRG_API ADrgPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADrgPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Input")
	TObjectPtr<UInputAction> AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Input")
	TObjectPtr<UInputAction> PauseAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Input")
	FGameplayTag AttackInputTag;

private:
	void Move(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
	void TogglePause(const FInputActionValue& Value);
};
