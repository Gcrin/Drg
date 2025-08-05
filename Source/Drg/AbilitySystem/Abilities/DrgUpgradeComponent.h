// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemInterface.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgAbilityDataAsset.h"
#include "DrgUpgradeComponent.generated.h"

class UAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DRG_API UDrgUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDrgUpgradeComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability")
	TArray<TObjectPtr<UDrgAbilityDataAsset>> AllAvailableAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Ability")
	TMap<TObjectPtr<UDrgAbilityDataAsset>, FGameplayAbilitySpecHandle> OwnedAbilityHandles;

public:
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	TArray<FDrgUpgradeChoice> GetLevelUpChoices(int32 NumChoices = 3);
	
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	void ApplyUpgradeChoice(const FDrgUpgradeChoice& SelectedChoice);

	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	void RemoveAbilityByData(UDrgAbilityDataAsset* AbilityData);
};
