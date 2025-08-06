// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrgGameplayAbility.h"
#include "DrgGA_Death.generated.h"

class UAnimMontage;
class UGameplayEffect;
class ADrgBaseCharacter;

USTRUCT(BlueprintType)
struct FOwnerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<const ADrgBaseCharacter> OwningCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAnimMontage> DeathMontage = nullptr;
};

UCLASS()
class DRG_API UDrgGA_Death : public UDrgGameplayAbility
{
	GENERATED_BODY()

public:
	UDrgGA_Death();

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;
	UFUNCTION(BlueprintPure, Category = "Drg|Ability")
	FOwnerData GetOwnerData() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|GAS")
	TSubclassOf<UGameplayEffect> DeathEffect;
};
