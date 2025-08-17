// Fill out your copyright notice in the Description page of Project Settings.


#include "DrgDropManagerSubsystem.h"

#include "DrgGameplayTags.h"
#include "Drg/Interfaces/DroppableInterface.h"
#include "Drg/Items/Data/DrgPickupDataAsset.h"
#include "Drg/Items/InstancedPickupManager.h"

void UDrgDropManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 게임의 모든 드롭 규칙이 정의된 중앙 레지스트리 데이터 애셋 로드
	const FSoftObjectPath DropRegistryPath(TEXT("/Game/Item/Data/DA_DropRegistry.DA_DropRegistry"));
	// TODO: 경로를 하드코딩하는 대신 프로젝트 설정 등에서 가져오게 변경하기
	DropRegistryDataAsset = Cast<UDropRegistryDataAsset>(DropRegistryPath.TryLoad());
	check(DropRegistryDataAsset != nullptr);
}

void UDrgDropManagerSubsystem::Deinitialize()
{
	// 레벨 종료 시, 등록했던 메시지 리스너를 해제하여 메모리 누수 방지
	if (CachedMessageSubsystem.IsValid() && ListenerHandle.IsValid())
	{
		CachedMessageSubsystem->UnregisterListener(ListenerHandle);
	}
	Super::Deinitialize();
}

void UDrgDropManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	CachedMessageSubsystem = &UGameplayMessageSubsystem::Get(&InWorld);

	if (CachedMessageSubsystem.IsValid())
	{
		ListenerHandle = CachedMessageSubsystem->RegisterListener(
			DrgGameplayTags::Event_Broadcast_ActorDied,
			this,
			&UDrgDropManagerSubsystem::OnActorDeathMessageReceived
		);
	}
}

void UDrgDropManagerSubsystem::OnActorDeathMessageReceived(FGameplayTag Channel, const FDrgActorDeathMessage& Message)
{
	// 메시지에 담겨온 죽은 액터(Victim) 유효성 확인.
	AActor* DeadActor = Message.Victim;
	if (!IsValid(DeadActor)) return;

	// 죽은 액터가 아이템을 드롭할 인터페이스를 가졌는지 확인.
	if (!DeadActor->Implements<UDroppableInterface>())
	{
		return;
	}

	// 인터페이스를 통해 죽은 액터의 드롭 태그 획득.
	const IDroppableInterface* DroppableActor = Cast<IDroppableInterface>(DeadActor);
	const FGameplayTag DropTag = DroppableActor->GetDropTag();
	if (!DropTag.IsValid())
	{
		return;
	}

	// 드롭 레지스트리(TMap)에서 해당 태그에 맞는 드롭 테이블 애셋 포인터 탐색.
	const TSoftObjectPtr<UDropTableDataAsset>* DropTableAssetPtr = DropRegistryDataAsset->DropRegistryMap.Find(DropTag);
	if (!DropTableAssetPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT(
			       "DropManager: 액터 '%s'에서 드롭 태그 '%s'를 찾았지만, DA_DropRegistry에 해당 항목이 없습니다."
		       ), *DropTag.ToString(), *DeadActor->GetName());
		return;
	}

	// 드롭 테이블 데이터 애셋을 동기 로드.
	UDropTableDataAsset* DropDataAsset = DropTableAssetPtr->LoadSynchronous();
	if (!DropDataAsset)
	{
		// 레지스트리에 항목은 있으나, 데이터 애셋이 할당되지 않은 경우.
		UE_LOG(LogTemp, Warning, TEXT(
			       "DropManager: 드롭 태그 '%s'는 DA_DropRegistry에 항목이 있지만, DropTableAsset이 설정되지 않았습니다."
		       ), *DropTag.ToString());
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector SpawnLocation = DeadActor->GetActorLocation();

	// 드롭 테이블에 정의된 모든 아이템 목록을 순회
	for (const FDrgDropItemInfo& DropInfo : DropDataAsset->PossibleDrops)
	{
		// 드롭 확률 체크
		const float Probability = FMath::Clamp(DropInfo.Probability, 0.0f, 1.0f);
		if (Probability <= 0.0f)
		{
			continue;
		}
		if (Probability == 1.0f || FMath::FRand() < Probability)
		{
			SpawnItems(DropInfo, SpawnLocation, World);
		}
	}
}

void UDrgDropManagerSubsystem::SpawnItems(const FDrgDropItemInfo& DropInfo, const FVector& Location, UWorld* World)
{
	// 드롭 정보에서 데이터 애셋을 로드
	UDrgPickupDataAsset* PickupData = DropInfo.PickupDataAsset.LoadSynchronous();
	if (!PickupData)
	{
		UE_LOG(LogTemp, Warning, TEXT("DropManager: DropInfo에 PickupDataAsset이 설정되지 않았습니다."));
		return;
	}

	// 해당 데이터 애셋을 처리하는 InstancedPickupManager가 이미 있는지 확인
	AInstancedPickupManager* Manager;
	TObjectPtr<AInstancedPickupManager>* FoundManager = InstancedManagers.Find(PickupData);

	if (FoundManager)
	{
		Manager = *FoundManager;
	}
	else
	{
		FActorSpawnParameters SpawnParams;
		Manager = World->SpawnActor<AInstancedPickupManager>(
			AInstancedPickupManager::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParams);
		Manager->Initialize(PickupData);
		InstancedManagers.Add(PickupData, Manager);
	}

	check(Manager);

	// 드롭 개수만큼 인스턴스를 추가하도록 Manager에게 요청.
	const int32 NumToSpawn = FMath::RandRange(DropInfo.DropCount.X, DropInfo.DropCount.Y);
	for (int32 i = 0; i < NumToSpawn; ++i)
	{
		const float SpawnRadius = 75.f;
		const FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0.f, SpawnRadius);
		const FVector BaseSpawnLocation = Location + RandomOffset;

		// === 바닥 찾기 LineTrace ===
		FVector GroundLocation = BaseSpawnLocation;
        
		FHitResult HitResult;
		FVector TraceStart = BaseSpawnLocation + FVector(0, 0, 500.0f);
		FVector TraceEnd = BaseSpawnLocation - FVector(0, 0, 500.0f);
        
		FCollisionQueryParams QueryParams;
		QueryParams.bTraceComplex = false;
		QueryParams.bReturnPhysicalMaterial = false;
        
		if (World->LineTraceSingleByChannel(
			HitResult, 
			TraceStart, 
			TraceEnd, 
			ECC_WorldStatic, 
			QueryParams))
		{
			GroundLocation = HitResult.Location;
		}

		FTransform FinalTransform = PickupData->LocalTransform;
		FinalTransform.AddToTranslation(GroundLocation);

		Manager->AddInstance(FTransform(FinalTransform));
	}
}
