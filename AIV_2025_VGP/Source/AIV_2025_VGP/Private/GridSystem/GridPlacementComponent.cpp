// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele

#include "GridSystem/GridPlacementComponent.h"

// Sets default values for this component's properties
UGridPlacementComponent::UGridPlacementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UGridPlacementComponent::PlaceTrap (FHitResult HitResult, const FName TrapRowName)
{
	//get closer surface
	FGridSurface* CloserSurface = GetCloserSurface(HitResult, TrapRowName);
	//check if surface is valid
	if(CloserSurface)
	{
		//set the surface occupied
		CloserSurface->bOccupied = true;
		//DrawDebug(CloserSurface);
		//Place Turret
	}
}
