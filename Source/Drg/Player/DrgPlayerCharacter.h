// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "DrgPlayerCharacter.generated.h"

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

protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|AbilityComp")
	TObjectPtr<UDrgUpgradeComponent> AbilityUpgradeComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Drg|Data")
	UDataTable* MaxExperienceDataTable;

	virtual void HandleOnLevelUp(AActor* Actor);

	virtual void PossessedBy(AController* NewController) override;
};
