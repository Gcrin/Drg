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

	UFUNCTION(blueprintCallable, Category = "Drg|CharacterState")
	void DeactivateCharacter();
	UFUNCTION(blueprintCallable, Category = "Drg|CharacterState")
	void ActivateCharacter();

protected:
	virtual void BeginPlay() override;

	// 캐릭터 데이터 적용
	void ApplyCharacterData();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	// GAS 초기화
	virtual void InitializeAttributes();
	virtual void GrantAbilities();
	void ResetAbilitySystemComponent();

	virtual void HandleOnMoveSpeedChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Data", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UDrgCharacterData> CharacterData;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|AbilitySystem")
	TObjectPtr<UDrgAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|AbilitySystem")
	TObjectPtr<UDrgAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, Category = "Drg|CharacterState")
	bool bIsDead;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|CharacterState")
	bool bIsAIControlled = false;
	// AI 컨트롤러를 저장할 변수
	UPROPERTY(Transient)
	TObjectPtr<class ADrgAIController> CachedAIController;
};
