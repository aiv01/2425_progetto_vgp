// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem/GridPlacementComponent.h"

// Sets default values for this component's properties
UGridPlacementComponent::UGridPlacementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UGridPlacementComponent::PlaceTrap (FGridSurface* GridSurface)
{
	//Place Turret
	GridSurface->bOccupied = true;
	
}
