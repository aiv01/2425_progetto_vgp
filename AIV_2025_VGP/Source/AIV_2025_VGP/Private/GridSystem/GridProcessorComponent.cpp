// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem/GridProcessorComponent.h"

#include "GridSystem/GridGeneratorVolume.h"

void UGridProcessorComponent::InitializeComponent ()
{
	Super::InitializeComponent();
	//check if the owner is from "GridGeneratorVolume" class
	/*if( GetOwner()->IsA(AGridGeneratorVolume::StaticClass()))
	{
		//link ref
		GridVolumeOwner = Cast<AGridGeneratorVolume>(GetOwner());
		if(GridVolumeOwner)		UE_LOG(LogTemp, Log, TEXT("owner setted'"));

	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner is not a 'AGridGeneratorVolume'"));
	}*/
}

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

FGridSurface* UGridProcessorComponent::GetCloserSurface (const FHitResult HitResult)
{
	if(!GridVolumeOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("GRID OWNER REF NULL"))
		return nullptr;
		
	}
	//AGridGeneratorVolume* GridVolumeOwner = Cast<AGridGeneratorVolume>(GetOwner());
	//FVector LocalHitLocation = GridVolumeOwner->GetOrigin() - HitResult.Location;

	//float DistanceCloserSurface = INT_MAX;
	//FVector VecDistanceCloserSurf = {INT_MAX, INT_MAX, INT_MAX};
	//float HalfCellSize = (GridVolumeOwner->CellSize)/2;
	//HalfCellSize*=HalfCellSize;
	for (auto& GridSurface : GridVolumeOwner->GetGridData())
	{
		//check the distance
		if(GridVolumeOwner->IsPointInsideGridSurface(GridSurface, HitResult.Location))
		{
			return &GridSurface;
		}
		
		/*
		float DistanceGridSurface = FVector::DistSquared(HitResult.Location, GridSurface.Position);

		if(DistanceCloserSurface > DistanceGridSurface)
		{
			DistanceCloserSurface = DistanceGridSurface;
			if(DistanceCloserSurface < HalfCellSize)
			{
				return &GridSurface;
			}
		}
		//check if it has the same rotation has the normal of the FHitResult
		if(FMath::IsNearlyEqual(FVector::DotProduct(HitResult.Normal, GridSurface.Orientation), 1.0f, KINDA_SMALL_NUMBER))
		{
			
		}*/
	}
	UE_LOG(LogTemp, Error, TEXT("NO SURFACE FOUND?"))

	return nullptr;
}

void UGridProcessorComponent::DrawDebug (const FGridSurface* CloserSurface) const
{
	//const FVector BoxExtent = CloserSurface->Orientation;
	const float HalfCell = GridVolumeOwner->CellSize * 0.5;
	DrawDebugBox(GetWorld(), CloserSurface->Position + (CloserSurface->Orientation * HalfCell), FVector{HalfCell}, {0,255,0}, false, 5, 0, 5);
}