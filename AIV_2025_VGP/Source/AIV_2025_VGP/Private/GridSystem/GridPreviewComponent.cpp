// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele
#include "GridSystem/GridPreviewComponent.h"

/**
 * Show the Preview of the trap mesh
 * @param HitResult Hit Location
 * @param MeshMaterial
 * @param CloserSurface set the FGridSurface 
 */
void UGridPreviewComponent::ShowPreview(const FHitResult& HitResult, UMaterial* MeshMaterial, FGridSurface*& CloserSurface)
{
	
	//set closer surface ref
	CloserSurface = GetCloserSurface(HitResult);
	//check if surface is valid
	if(CloserSurface)
	{
		if(!MeshMaterial)
		{
			UE_LOG(LogTemp, Error, TEXT("MATERIAL NOT SET IN GRID INTERACT COMPONENT"));
			return;
		}
		
		//DrawDebug(CloserSurface);
		//Place preview Turret
	}
	
}
