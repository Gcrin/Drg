// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "DroppableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDroppableInterface : public UInterface
{
	GENERATED_BODY()
};

class DRG_API IDroppableInterface
{
	GENERATED_BODY()

public:
	// 이 액터의 드롭 규칙을 식별하는 GameplayTag를 반환 
	virtual FGameplayTag GetDropTag() const = 0;
};
