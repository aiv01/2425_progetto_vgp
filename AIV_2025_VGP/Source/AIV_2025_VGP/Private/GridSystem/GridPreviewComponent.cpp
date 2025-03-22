// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele
#include "GridSystem/GridPreviewComponent.h"

/**
 * Show the Preview of the trap mesh
 * @param HitResult Hit Location
 * @param CloserSurface set the FGridSurface 
 */
void UGridPreviewComponent::ShowPreview (const FHitResult HitResult, FGridSurface*& CloserSurface)
{
	//set closer surface ref
	CloserSurface = GetCloserSurface(HitResult);
	//check if surface is valid
	if(CloserSurface)
	{
		DrawDebug(CloserSurface);
		//Place preview Turret
	}
	
}
