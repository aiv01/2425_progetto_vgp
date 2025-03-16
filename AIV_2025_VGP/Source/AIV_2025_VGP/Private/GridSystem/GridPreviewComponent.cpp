// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem/GridPreviewComponent.h"

void UGridPreviewComponent::ShowPreview (const FHitResult HitResult, FGridSurface*& CloserSurface)
{
	CloserSurface = GetCloserSurface(HitResult);
	if(CloserSurface)
	{
		DrawDebug(CloserSurface);
		//Place Turret
	}
	
}
