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

/**
 * 
 */
UCLASS()
class DRG_API UDrgAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UDrgAttributeSet();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	FOnDeath OnDeath;
	
	UPROPERTY(BlueprintReadOnly, Category = "Drg|Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Drg|Movement")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UDrgAttributeSet, MoveSpeed);
};
