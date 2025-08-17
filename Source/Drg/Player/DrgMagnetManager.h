// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DrgMagnetManager.generated.h"

class USceneComponent;
class UBillboardComponent;

/*
 * @brief 캐릭터 주변의 아이템을 탐색해 빨아들이는 액터
 */
UCLASS()
class DRG_API ADrgMagnetManager : public AActor
{
	GENERATED_BODY()
	
public:
	ADrgMagnetManager();
	
	virtual void Tick(float DeltaTime) override;

	void SetMagnetRadius(float NewRadius);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drg|Magnet")
	float PullSpeed = 1000.0f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drg|Magnet")
	float MagnetRadius = 100.0f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;
};
