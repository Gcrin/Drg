// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "DrgPlayerCharacter.generated.h"

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

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

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

private:
	UFUNCTION()
	void OnSkillSelected(int32 SkillIndex);

	void CheckLevelUp();

	int32 UpgradeCount = 0;
	FTimerHandle LevelUpTimerHandle;
};
