// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgAnimNotify_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"


void UDrgAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, FGameplayEventData());
	}
}
