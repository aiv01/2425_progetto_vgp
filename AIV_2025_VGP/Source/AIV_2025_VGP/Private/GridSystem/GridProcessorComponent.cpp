// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem/GridProcessorComponent.h"

#include "GridSystem/GridGeneratorVolume.h"


void UGridProcessorComponent::BeginPlay ()
{
	Super::BeginPlay();
	if( GetOwner()->IsA(AGridGeneratorVolume::StaticClass()))
	{
		//link ref
		GridVolumeOwner = Cast<AGridGeneratorVolume>(GetOwner());
		if(GridVolumeOwner)		UE_LOG(LogTemp, Log, TEXT("owner setted'"));

	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner is not a 'AGridGeneratorVolume'"));
	}
}

FGridSurface* UGridProcessorComponent::GetCloserSurface (const FHitResult HitResult,  const FName TrapName)
{
	if(!GridVolumeOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("GRID OWNER REF NULL"))
		return nullptr;
		
	}
	
	for (auto& GridSurface : GridVolumeOwner->GetGridData())
	{
		
		//check the distance
		if(GridVolumeOwner->IsPointInsideGridSurface(GridSurface, HitResult.Location))
		{
			return &GridSurface;
		}
		
		
	}
	UE_LOG(LogTemp, Error, TEXT("NO SURFACE FOUND?"))

	return nullptr;
}

void UGridProcessorComponent::DrawDebug (const FGridSurface* CloserSurface) const
{
	const float HalfCell = GridVolumeOwner->GetHalfCellSize();
	DrawDebugBox(GetWorld(), CloserSurface->Position + (CloserSurface->Orientation * HalfCell), FVector{HalfCell}, {0,255,0}, false, 0.2, 0, 5);
	DrawDebugDirectionalArrow(GetWorld(), CloserSurface->Position, CloserSurface->Position + CloserSurface->Orientation * 100, 25, {255,0,0}, false, 0.2, 0, 10);
}

FRotator UGridProcessorComponent::GetTrapRotation (FVector CloserSurfaceOrientation)
{
	FVector Forward = CloserSurfaceOrientation.GetSafeNormal(); // Assicura che sia un vettore unitario
	FVector Right = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal(); // Ortogonale a Forward
	FVector Up = FVector::CrossProduct(Forward, Right); // Ortogonale a entrambi
	FMatrix RotationMatrix = FMatrix(Forward, Right, Up, FVector::ZeroVector);
	FRotator TargetRotation = RotationMatrix.Rotator();	
	TargetRotation.Pitch -= 90.0f;
	return TargetRotation;
}