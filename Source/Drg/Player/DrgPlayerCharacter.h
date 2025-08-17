// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "Drg/GameModes/DrgMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "DrgPlayerCharacter.generated.h"

class ADrgMagnetManager;
class UDrgSkillSelectionWidget;
class UCameraComponent;
class USpringArmComponent;
class UDrgUpgradeComponent;

/**
 * 
 */
UCLASS()
class DRG_API ADrgPlayerCharacter : public ADrgBaseCharacter
{
	GENERATED_BODY()

public:
	ADrgPlayerCharacter();

	UDataTable* GetDataTable() const;

	virtual void HandleOnLevelUp(AActor* Actor);

	virtual void DeactivateCharacter() override;
	virtual void ActivateCharacter() override;

	UFUNCTION(BlueprintCallable, Category = "Drg|Appearance")
	void SetCharacterAppearance(USkeletalMesh* NewMesh, const TArray<UMaterialInterface*>& NewMaterials);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitializeAttributes() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|AbilityComp")
	TObjectPtr<UDrgUpgradeComponent> AbilityUpgradeComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Drg|Data")
	UDataTable* MaxExperienceDataTable;

	// UI 관련 추가
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|UI")
	TSubclassOf<UDrgSkillSelectionWidget> SkillSelectionWidgetClass;

	UPROPERTY()
	TObjectPtr<UDrgSkillSelectionWidget> SkillSelectionWidget;

	// 아이템 자석 효과 매니저 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|MagnetManager")
	TSubclassOf<ADrgMagnetManager> MagnetManagerClass;

private:
	UFUNCTION()
	void OnSkillSelected(int32 SkillIndex);
	void CheckLevelUp();
	void EndLevelUpSequence();

	int32 UpgradeCount = 0;
	FTimerHandle LevelUpTimerHandle;
	bool bIsLevelUpSequence = false;

	// 아이템 자석
	UPROPERTY()
	TObjectPtr<ADrgMagnetManager> MagnetManager;
	void UpdateMagnetRadius();
	void OnAttributeChanged(FGameplayTag Channel, const FDrgAttributeChangeMessage& Message);
	FGameplayMessageListenerHandle AttributeChangedListenerHandle;
};
