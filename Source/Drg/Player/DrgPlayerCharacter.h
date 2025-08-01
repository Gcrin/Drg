// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "DrgPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UDrgAbilityDataAsset;

USTRUCT(BlueprintType)
struct FDrgUpgradeChoice
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Ability")
	TObjectPtr<UDrgAbilityDataAsset> AbilityData = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Ability")
	bool bIsUpgrade = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Ability")
	int32 PreviousLevel = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drg|Ability")
	int32 NextLevel = 0;
};
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

protected:

	virtual void PossessedBy(AController* NewController) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Drg|Data")
	UDataTable* MaxExperienceDataTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drg|Ability")
	TArray<TObjectPtr<UDrgAbilityDataAsset>> AllAvailableAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Ability")
	TMap<TObjectPtr<UDrgAbilityDataAsset>, FGameplayAbilitySpecHandle> OwnedAbilityHandles;

public:
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	TArray<FDrgUpgradeChoice> GetLevelUpChoices(int32 NumChoices = 3);
	
	UFUNCTION(BlueprintCallable, Category = "Drg|Ability")
	void ApplyUpgradeChoice(const FDrgUpgradeChoice& SelectedChoice); 
};
