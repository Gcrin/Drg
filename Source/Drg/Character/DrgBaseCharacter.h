// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DrgBaseCharacter.generated.h"

UCLASS()
class DRG_API ADrgBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADrgBaseCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Camera")
	TObjectPtr<class UCameraComponent> CameraComponent;
};
