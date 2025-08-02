// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drg/Character/DrgBaseCharacter.h"
#include "DrgPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
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

	AActor* GetTargetAcotr() const;
	void SetTargetActor(AActor* pTargetActor);

	UFUNCTION(BlueprintCallable, Category="Target",
		meta = (DisplayName = "Find Target Actor", ToolTip = "타겟을 찾아 멤버변수로 저장"))
	void FindTargetActor();

protected:
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Drg|Data")
	UDataTable* MaxExperienceDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Drg|TargetActor")
	float TraceDistance = 1000.f;

private:
	//투사체가 가져가야할 TargetActor
	AActor* TargetActor = nullptr;
};
