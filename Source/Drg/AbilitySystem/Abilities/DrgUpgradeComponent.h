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
	/**
	* @brief 필요한 개수의 랜덤한 어빌리티들의 데이터를 반환해주는 함수입니다. 
	* @param NumChoices 반환 받을 어빌리티 선택지의 개수입니다.
	* @return 반환될 어빌리티가 모자랄 경우, Default 값으로 채웁니다. DataAsset = nullptr
	*/
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	TArray<FDrgUpgradeChoice> GetLevelUpChoices(int32 NumChoices = 3);
	
	// @brief 선택된 어빌리티를 플레이어에 적용해 주는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	void ApplyUpgradeChoice(const FDrgUpgradeChoice& SelectedChoice);

	// @brief 플레이어가 소유한 어빌리티를 삭제하는 함수입니다. 
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	void RemoveAbilityByData(UDrgAbilityDataAsset* AbilityData);
};
