// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "DrgExperienceManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, Level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExperienceGained, float, Experience);

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class DRG_API UDrgExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()


public:
	UDrgExperienceManagerComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	// 함수 세분화 필요
	UFUNCTION(BlueprintCallable, Category = "ExperienceManager")
	void AddExperience(float Amount);
	
	UFUNCTION(BlueprintPure, Category = "ExperienceManager")
	int32 GetCurrentLevel() const;
	UFUNCTION(BlueprintPure, Category = "ExperienceManager")
	float GetExperience() const;

	// 레벨 업 -> 이펙트, 업그레이드 등 // 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnLevelUp OnLevelUp;
	// 경험치 획득 시 -> 이펙트, 사운드 등 // 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnExperienceGained OnExperienceGained;
	
	// 데이터 테이블로 변경
	UPROPERTY(EditDefaultsOnly, Category = "ExperienceManager")
	float MaxExperience;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "ExperienceManager")
	int32 CurrentLevel;
	UPROPERTY(BlueprintReadOnly, Category = "ExperienceManager")
	float CurrentExperience;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
