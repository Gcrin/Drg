// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "DrgBaseCharacter.generated.h"

class UDrgAttributeSet;
class UDrgAbilitySystemComponent;

UCLASS()
class DRG_API ADrgBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADrgBaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	// GAS 초기화
	virtual void InitializeAttributes();
	virtual void GrantAbilities();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|AbilitySystem")
	TObjectPtr<UDrgAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|AbilitySystem")
	TObjectPtr<UDrgAttributeSet> AttributeSet;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|AbilitySystem")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|AbilitySystem")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<class UCameraComponent> CameraComponent;
};
