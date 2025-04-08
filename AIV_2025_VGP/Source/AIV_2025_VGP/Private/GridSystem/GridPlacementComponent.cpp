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

bool UGridPlacementComponent::PlaceTrap (FHitResult HitResult, const FName TrapRowName)
{
	// Get the closest surface from the hit result
	FGridSurface* CloserSurface = GetCloserSurface(HitResult, TrapRowName);
	//check if surface is valid
	if(CloserSurface && !CloserSurface->bOccupied)
	{
		// Retrieve trap data from the volume using the TrapRowName
		FTrapData* TrapData = GridVolumeOwner->GetTrapData(TrapRowName);
		if(!(TrapData && TrapData->TrapClass))
		{
			UE_LOG(LogTemp, Warning, TEXT("Trap not setted in datatable or name wrong"))
			return false;
		}

		if((TrapData->Type.Contains(ETrapType::Floor) && CloserSurface->Orientation.Z != 1) ||
			(TrapData->Type.Contains(ETrapType::Wall) && CloserSurface->Orientation.X == 0 && CloserSurface->Orientation.Y == 0))
		{
			//cant place turret, wrong surgace
			return false;	
		}
		
		//Spawn the Trap
		AActor* Trap = GetWorld()->SpawnActor<AActor>(TrapData->TrapClass);
		// Set Trap Position, Rotation and Scale
		Trap->SetActorLocation(CloserSurface->Position);
		Trap->SetActorRotation(GetTrapRotation(CloserSurface->Orientation));
		Trap->SetActorScale3D(FVector{ GridVolumeOwner->GetHalfCellSize() / 50.f });
		
		//set the surface occupied
		CloserSurface->bOccupied = true;
		return true;
	}
	return false;
}
