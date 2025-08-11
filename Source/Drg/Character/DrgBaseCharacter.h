// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Drg/AbilitySystem/DrgAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "DrgBaseCharacter.generated.h"

class UDrgCharacterData;
class UDrgAttributeSet;
class UDrgAbilitySystemComponent;
class ADrgProjectile;
class UDrgOrbitalMovementComponent;

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

	void SetCharacterData(const TObjectPtr<UDrgCharacterData>& NewCharacterData)
	{
		this->CharacterData = NewCharacterData;
	}

	TObjectPtr<UDrgCharacterData> GetCharacterData() const
	{
		return CharacterData;
	}

	UFUNCTION(blueprintCallable, Category = "Drg|CharacterState")
	virtual void DeactivateCharacter();
	UFUNCTION(blueprintCallable, Category = "Drg|CharacterState")
	virtual void ActivateCharacter();
	UFUNCTION()
	virtual void OnDeathCleanup();


protected:
	virtual void BeginPlay() override;

	// 캐릭터 데이터 적용
	void ApplyCharacterData();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	// GAS 초기화
	virtual void InitializeAttributes();
	virtual void GrantAbilities();

	virtual void HandleOnMoveSpeedChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Data", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UDrgCharacterData> CharacterData;

	//회전투사체 중심축
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<USceneComponent> OrbitPivotComponent;
	// 실제 투사체 관리와 회전 로직을 담당하는 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Components")
	TObjectPtr<UDrgOrbitalMovementComponent> OrbitalMovementComponent;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|AbilitySystem")
	TObjectPtr<UDrgAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|AbilitySystem")
	TObjectPtr<UDrgAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|CharacterState")
	bool bIsAIControlled = false;
};
