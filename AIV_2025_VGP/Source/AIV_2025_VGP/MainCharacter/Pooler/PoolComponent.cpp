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
	UE_LOG(LogTemp, Warning, TEXT("GetElement"));
	for(AActor* SpawnedActor : PoolList)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryGetActor"));

		if (SpawnedActor && SpawnedActor->IsHidden())
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor: %s"), *SpawnedActor->GetName());
			UE_LOG(LogTemp, Warning, TEXT("Setting Rotation: %s"), *Rotation.ToCompactString());
			SpawnedActor->SetActorLocation(Location);
			SpawnedActor->SetActorRotation(Rotation);
			UE_LOG(LogTemp, Warning, TEXT("Setting Rotation: %s"), *Rotation.ToCompactString());
			SpawnedActor->SetActorHiddenInGame(false);
			SpawnedActor->SetActorEnableCollision(true);

			// if (UProjectileMovementComponent* Movement = SpawnedActor->FindComponentByClass<UProjectileMovementComponent>())
			// {
			// 	Movement->Velocity = SpawnedActor->GetActorForwardVector() * 2000;
			// 	Movement->Activate(true);
			// }

			return SpawnedActor;
		}
	}

	return nullptr;
}
