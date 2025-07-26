// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "DrgBaseCharacter.generated.h"

class UDrgCharacterData;
class UDrgAttributeSet;
class UDrgAbilitySystemComponent;

UCLASS()
class DRG_API ADrgBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADrgBaseCharacter();

	UFUNCTION(BlueprintPure, Category = "Drg|CharacterState")
	bool IsDead() const;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	TObjectPtr<UDrgAttributeSet> GetAttributeSet() const;

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	// GAS 초기화
	virtual void InitializeAttributes();
	virtual void GrantAbilities();

	virtual void HandleOnDeath(AActor* DeadActor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Data")
	TObjectPtr<UDrgCharacterData> CharacterData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|AbilitySystem")
	TObjectPtr<UDrgAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|AbilitySystem")
	TObjectPtr<UDrgAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, Category = "Drg|CharacterState")
	bool bIsDead;
};
