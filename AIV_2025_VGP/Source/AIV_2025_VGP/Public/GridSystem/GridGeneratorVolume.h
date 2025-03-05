// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "GridGeneratorVolume.generated.h"

#define ERROR_MARGIN 10.f

struct FGridSurface;
enum EObjectTypeQuery;
enum ESurfaceOrientation;
enum ECollisionChannel;
/**
 * 
 */
UCLASS(Blueprintable)
class AIV_2025_VGP_API AGridGeneratorVolume : public AVolume
{
	GENERATED_BODY()
public:
	// "button" to start generation
	UFUNCTION(CallInEditor, Category="GridGenerator")
	void Generate();

protected:
	// size of the grid cells
	UPROPERTY(EditInstanceOnly, Category="GridGenerator|Parameters")
	float CellSize;

	// types of objects that can interfere with grid generation
	UPROPERTY(EditInstanceOnly, Category="GridGenerator|Parameters")
	TArray<TEnumAsByte<EObjectTypeQuery>> DisruptingObjectTypes;

	// how much a cell can be occupied. 1 means that a cell can be generated even inside a wall.
	// calculated from the center of the cell
	UPROPERTY(EditInstanceOnly, Meta=(ClampMin=0.f, ClampMax=1.f), Category = "GridGenerator|Parameters")
	float MaxCellOccupation;

	// minimum of available surface to generate point (starting from center of the surface)
	// 1 means that no corner must me missing, 0 means that a surface can be generated even in mid air
	UPROPERTY(EditInstanceOnly, Meta=(ClampMin = 0.f, ClampMax = 1.f), Category = "GridGenerator|Parameters")
	float MinAvailableSurface;

	// slope acceptance threshold
	UPROPERTY(EditInstanceOnly, Meta=(ClampMin=0.f, ClampMax=45.f), Category = "GridGenerator|Parameters")
	float MaxSlopeAcceptance;

	// array of the surfaces orientations on which we want to generate surfaces
	// Up means that it will generate surfaces on the floor (1, 0, 0) 
	// Right means that it will generate surfaces with a y positive orientation (0, 1, 0)
	UPROPERTY(EditInstanceOnly, Category = "GridGenerator|Parameters")
	TArray<TEnumAsByte<ESurfaceOrientation>>  Directions;

	// collision types on which we want to generate surfaces
	UPROPERTY(EditInstanceOnly, Category = "GridGenerator|Parameters")
	TSet<TEnumAsByte<ECollisionChannel>> SurfaceTypes;


	// not editable, only for info purposes. Depends on cell size and extents
	UPROPERTY(VisibleAnywhere, Category = "GridGenerator|Info")
	FIntVector SizeInCells;

	// not editable, only for info purposes. Depends on x y extents
	UPROPERTY(VisibleAnywhere, Category = "GridGenerator|Info")
	uint32 CellsInHSection;

	// not editable only for info purposes. Depends on x y z extents
	UPROPERTY(VisibleAnywhere, Category = "GridGenerator|Info")
	uint32 TotalCells;

	// container of all the surfaces in the grid
	UPROPERTY(EditAnywhere, Category = "GridGenerator")
	TArray<FGridSurface> GridData;

	float HalfCellSize;

	// defines direction of square angles (relative to the vector (1, 0, 0))
	TArray<FVector> Angles = { FVector(0, 1, 1), FVector(0, -1, 1), FVector(0, 1, -1), FVector(0, -1, -1)};

private:
	// self explanatory...
	void CalculateSizeInCells(const FVector& VolumeSize);

	// the function that iterates grid cells calculating their positions
	void GenerateGrid(const FVector& Origin);

	// returns the x position of the cell at CurrentIndex
	float GetCellXPosition(const uint32& CurrentIndex) const;

	// returns the y position of the cell at CurrentIndex
	float GetCellYPosition(const uint32& CurrentIndex) const ;

	// returns the z position of the cell at CurrentIndex
	float GetCellZPosition(const uint32& CurrentIndex) const;

	// generates the surfaces given a cell position
	void GenerateSurfaces(const FVector& CellPosition);

	// checks if the surface has enough "surface quantity" based on MinAvailableSurface
	// returns true if there is enough surface
	bool CheckCorners(const FVector& CellPosition, const FVector& SurfaceDirection) const;
};
