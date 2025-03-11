// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveSpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

AWaveSpawner::AWaveSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	RootComponent = SpawnBox;
	
	SpawnBox->SetBoxExtent(FVector(500.0f, 500.0f, 1.0f));
	
	SpawnWeight = 1.0f;
}

FVector AWaveSpawner::GetRandomSpawnLocation() const
{
	FVector Origin = SpawnBox->Bounds.Origin;
	FVector Extent = SpawnBox->GetUnscaledBoxExtent();
	
	FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
	
	RandomPoint.Z = Origin.Z;

	return RandomPoint;
}

