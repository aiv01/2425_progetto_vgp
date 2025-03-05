// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele

#include "GridSystem/GridGeneratorVolume.h"
#include "GridSystem/FGridSurface.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GridSystem/ESurfaceOrientation.h"

void AGridGeneratorVolume::Generate()
{
	// clear all of the data
	GridData.Empty();

	// get volume extents
	FVector VolumeExtents;
	VolumeExtents.X = Brush->Bounds.BoxExtent.X * GetActorScale().X;
	VolumeExtents.Y = Brush->Bounds.BoxExtent.Y * GetActorScale().Y;
	VolumeExtents.Z = Brush->Bounds.BoxExtent.Z * GetActorScale().Z;
	
	// get origin of the generation 
	FVector Origin = GetActorLocation() - VolumeExtents;

	// get whole size of the volume
	FVector VolumeSize = VolumeExtents * 2.f;

	// at this point we can calculate size in cells
	CalculateSizeInCells(VolumeSize);

	// start the actual grid generation
	GenerateGrid(Origin);
}

void AGridGeneratorVolume::CalculateSizeInCells(const FVector& VolumeSize)
{
	// calculate size in cells. It's a vector composed by only ints so we use floor
	// this will result in a rounding down
	SizeInCells.X = FMath::Floor(VolumeSize.X / CellSize);
	SizeInCells.Y = FMath::Floor(VolumeSize.Y / CellSize);
	SizeInCells.Z = FMath::Floor(VolumeSize.Z / CellSize);
}

void AGridGeneratorVolume::GenerateGrid(const FVector& Origin)
{
	// useful in various occasions
	HalfCellSize = CellSize * 0.5f;

	// total cells
	TotalCells = SizeInCells.X * SizeInCells.Y * SizeInCells.Z;

	// number of cells in a horizontal section of the volume
	CellsInHSection = SizeInCells.X * SizeInCells.Y;

	for (uint32 i = 0; i < TotalCells; i++)
	{
		// initialize current cell pos
		FVector CurrentCellPos = FVector::ZeroVector;

		// the starting position is always halfcellsize
		CurrentCellPos.X = CurrentCellPos.X + HalfCellSize;
		CurrentCellPos.Y = CurrentCellPos.Y + HalfCellSize;
		CurrentCellPos.Z = CurrentCellPos.Z + HalfCellSize;

		// calculate cell X position
		CurrentCellPos.X += GetCellXPosition(i);
		// calculate cell Y position
		CurrentCellPos.Y += GetCellYPosition(i);
		// caclutate cell Z position
		CurrentCellPos.Z += GetCellZPosition(i);

		TArray<AActor*> ToIgnore;
		TArray<AActor*> HitActors;
		//DrawDebugBox(GetWorld(), Origin + CurrentCellPos, FVector(HalfCellSize * MaxCellOccupation), FColor::Black, false, 5.f, 0U, 10.f);
		// check if cell is occupied (using parameter)
		if(UKismetSystemLibrary::BoxOverlapActors(GetWorld(), Origin + CurrentCellPos, FVector(HalfCellSize * MaxCellOccupation), DisruptingObjectTypes, NULL, ToIgnore, HitActors))
		{
			// don't generate point/surfaces
			continue;
		}
		// cell is valid, can generate surfaces
		GenerateSurfaces(Origin + CurrentCellPos);
	}

}

float AGridGeneratorVolume::GetCellXPosition(const uint32& CurrentIndex) const
{
	// calculate X coordinate in cell units;
	uint32 CellX = CurrentIndex % SizeInCells.X;
	// return the actual position (CellCoord * CellSize)
	return CellX * CellSize;
}

float AGridGeneratorVolume::GetCellYPosition(const uint32& CurrentIndex) const 
{
	// calculate Y coordinate in cell units
	uint32 CellY = (CurrentIndex % CellsInHSection) / SizeInCells.X;
	// return the actual position
	return CellY * CellSize;
}

float AGridGeneratorVolume::GetCellZPosition(const uint32& CurrentIndex) const
{
	// calculate Z coordinate in cell units
	uint32 CellZ = CurrentIndex / CellsInHSection;
	// return the actual position
	return CellZ * CellSize;
}

void AGridGeneratorVolume::GenerateSurfaces(const FVector& CellPosition)
{
	uint32 DirectionsLen = Directions.Num();

	// iterate on the user specified directions 
	for (uint32 i = 0; i < DirectionsLen; i++)
	{
		FVector CurrentTraceDir = FVector::ZeroVector;

		// set the current trace dir depending on the directions
		switch (Directions[i])
		{
			case(ESurfaceOrientation::XPOS):
				CurrentTraceDir = FVector::BackwardVector;
				break;
			case(ESurfaceOrientation::XNEG):
				CurrentTraceDir = FVector::ForwardVector;
				break;
			case(ESurfaceOrientation::YNEG):
				CurrentTraceDir = FVector::RightVector;
				break;
			case(ESurfaceOrientation::YPOS):
				CurrentTraceDir = FVector::LeftVector;
				break;
			case(ESurfaceOrientation::ZNEG):
				CurrentTraceDir = FVector::UpVector;
				break;
			case(ESurfaceOrientation::ZPOS):
				CurrentTraceDir = FVector::DownVector;
				break;
			default:
				break;
		}

		// cast line trace in the selected direction to check if we have a valid surface
		FVector SurfPosition = CellPosition + (CurrentTraceDir * HalfCellSize);
		FVector TraceEnd = SurfPosition + CurrentTraceDir * ERROR_MARGIN;
		FHitResult HitResult;
		// DrawDebugLine(GetWorld(), CellPosition, TraceEnd, FColor::Red, false, 5.f, 0U, 5);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, CellPosition, TraceEnd, ECollisionChannel::ECC_Visibility))
		{
			// check on hit object collision type
			if (!SurfaceTypes.Contains(HitResult.Component->GetCollisionObjectType()))
			{
				// hit object collision type not valid for surface generation
				continue;
			}

			// check on slope
			if (!(FMath::Acos(FVector::DotProduct(-HitResult.Normal, CurrentTraceDir)) <= MaxSlopeAcceptance))
			{
				// slope too steep
				continue;
			}

			// check on corners
			if (!CheckCorners(CellPosition, CurrentTraceDir))
			{
				// not enough surface space to generate surface
				continue;
			}
			
			// we can add the FGridSurface to the grid data
			FGridSurface CurrentSurfaceData;
			CurrentSurfaceData.bOccupied = false;
			CurrentSurfaceData.Orientation = -CurrentTraceDir;
			CurrentSurfaceData.Position = SurfPosition;

			GridData.Add(CurrentSurfaceData);
		}
	}
}

bool AGridGeneratorVolume::CheckCorners(const FVector& CellPosition, const FVector& SurfaceDirection) const
{
	// iterate al angles directions
	for (const FVector& Direction : Angles)
	{
		// get the rotation of the surface normal 
		FRotator Rotation = UKismetMathLibrary::MakeRotFromX(SurfaceDirection);
		// rotate the angle direction on the surface normal
		FVector RotatedVector = Rotation.RotateVector(Direction);
		// set its length depending on MinAvailableSurface
		RotatedVector *= HalfCellSize * MinAvailableSurface;
		// calculate line trace start
		FVector TraceStart = CellPosition + RotatedVector;
		// calculate line trace end
		FVector TraceEnd = TraceStart + (SurfaceDirection * (HalfCellSize + ERROR_MARGIN));

		// check to see if corner is missing
		FHitResult HitResult;
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Black, false, 5.f, 0U, 5);
		if (!GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility))
		{
			// corner missing
			return false;
		}
	}
	return true;
}
