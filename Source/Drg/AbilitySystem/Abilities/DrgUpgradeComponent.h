// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemInterface.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgAbilityDataAsset.h"
#include "Drg/AbilitySystem/Abilities/Data/DrgUpgradeChoice.h"
#include "DrgUpgradeComponent.generated.h"

class UAbilitySystemComponent;
class UDrgUpgradeDataCollection;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUpChoicesReady, const TArray<FDrgUpgradeChoice>&, Choices);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DRG_API UDrgUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDrgUpgradeComponent();

	UPROPERTY(BlueprintAssignable, Category = "Drg|Ability|Events")
	FOnLevelUpChoicesReady OnLevelUpChoiceReady;

	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	bool PresentLevelUpChoices(int32 NumChoices = 3);

	// 소유 가능한 최대 어빌리티 종류의 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Upgrade", meta=(ClampMin = "1"))
	int32 MaxAcquirableAbilityCount = 6;

protected:
	virtual void BeginPlay() override;

	void UpgradeAbility(const FDrgUpgradeChoice& SelectedChoice);
	void UpgradeEffect(const FDrgUpgradeChoice& SelectedChoice);

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability")
	TObjectPtr<UDrgUpgradeDataCollection> AbilityCollectionData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Ability")
	TMap<TObjectPtr<UDrgAbilityDataAsset>, FGameplayAbilitySpecHandle> OwnedAbilityHandles;

	UPROPERTY()
	TMap<TObjectPtr<UDrgAbilityDataAsset>, FActiveGameplayEffectHandle> ActiveEffectHandles;

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

	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	TArray<FDrgUpgradeChoice> StartAbilityChoices(int32 NumChoices = 3);
};
