// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values for this component's properties
UPoolComponent::UPoolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

// Called when the game starts
void UPoolComponent::BeginPlay()
{
	Super::BeginPlay();	
	// ...
	
}

void UPoolComponent::InitializePool()
{
	UE_LOG(LogTemp, Warning, TEXT("InitializePool"));
	if(!PoolElementClass)
	{
		return;
	}

	UWorld* world = GetWorld();
	if(!world)
	{
		return;
	}

	for (int32 i = 0; i < PoolSize; ++i)
	{
		AActor* SpawnedActor = world->SpawnActor<AActor>(PoolElementClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if(SpawnedActor)
		{
			SpawnedActor->SetActorEnableCollision(false);
			SpawnedActor->SetActorHiddenInGame(true);
			SpawnedActor->SetActorTickEnabled(false);
			UE_LOG(LogTemp, Warning, TEXT("SpawnedActor"));
			
			PoolList.Add(SpawnedActor);
		}
	}
}
AActor* UPoolComponent::GetPoolElement(FVector Location, FRotator Rotation)
{
	AActor* SpawnedActor = PoolList[0];
	
	if(SpawnedActor)
	{
		SpawnedActor->SetActorLocation(Location);
		SpawnedActor->SetActorRotation(Rotation);
		SpawnedActor->SetActorHiddenInGame(false);
		SpawnedActor->SetActorEnableCollision(true);

		// Infinity Loop for pooler
		PoolList.Remove(SpawnedActor);
		PoolList.Add(SpawnedActor);

		return SpawnedActor;
	}
	return nullptr;
}	
