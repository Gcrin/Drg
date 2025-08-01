// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "DrgAnimNotify_SendGameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class DRG_API UDrgAnimNotify_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;
	// 디자이너가 에디터에서 설정할 이벤트 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayEvent")
	FGameplayTag EventTag;
};
