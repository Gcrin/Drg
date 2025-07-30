// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DrgAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUp, AActor*);

/**
 * 
 */
UCLASS()
class DRG_API UDrgAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UDrgAttributeSet();

	/**
	 * @brief 어트리뷰트의 'BaseValue'가 변경되기 직전에 호출.
	 * @details 값 변경을 최종 적용하기 전, 값을 검증하거나 강제로 조정(Clamp)하는 데 사용.
	 *          (예: Health가 MaxHealth를 초과하지 않도록 방지)
	 * @param Attribute 변경되려는 어트리뷰트.
	 * @param NewValue 변경될 새로운 값. 이 값을 함수 내에서 직접 수정 가능.
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	/**
	 * @brief '즉시 효과(Instant)' 타입의 GameplayEffect가 적용된 직후 호출.
	 * @details 데미지, 회복 등 즉시 효과가 적용된 뒤 후처리(죽음 판정, 이펙트 재생 등)를 진행할 때 사용.
	 *          지속 효과(DoT 등)에서는 자동 호출되지 않으므로, Tick마다 직접 Instant 이펙트를 트리거하는 방식으로 처리해야 함.
	 * @param Data 적용된 GameplayEffect에 대한 상세 정보 (원인, 변경된 어트리뷰트 등 포함).
	 */
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	FOnDeath OnDeath;
	FOnDeath OnLevelUp;

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Health")
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, HealthRegen);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Damage")
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, AttackDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Damage")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, AttackSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Defense")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, Defense);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Movement")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, MoveSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Stamina")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, MaxStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Stamina")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Stamina")
	FGameplayAttributeData StaminaRegen;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, StaminaRegen);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Pickup")
	FGameplayAttributeData PickupRadius;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, PickupRadius);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Pickup")
	FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, CharacterLevel);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Pickup")
	FGameplayAttributeData MaxExperience;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, MaxExperience);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Pickup")
	FGameplayAttributeData Experience;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, Experience);
};
