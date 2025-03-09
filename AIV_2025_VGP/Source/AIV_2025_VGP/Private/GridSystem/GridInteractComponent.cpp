// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem/GridInteractComponent.h"






//Interface Bind Function For Preview
void UGridInteractComponent::BindPreviewInteract (UGridPreviewComponent* GridPreviewComponent)
{
	if(GridPreviewComponent)
	{
		OnShowPreviewTriggered.BindUObject(GridPreviewComponent, &UGridPreviewComponent::ShowPreview);
	}
}

//Interface Bind Function For Placement
void UGridInteractComponent::BindPlacementInteract (UGridPlacementComponent* GridPlacementComponent)
{
	if(GridPlacementComponent)
	{
		OnPlaceTrapTriggered.BindUObject(GridPlacementComponent, &UGridPlacementComponent::PlaceTrap);
	}
}

//call binded Show Preview Function
void UGridInteractComponent::ShowPreview (const FVector Point)
{
	if(OnShowPreviewTriggered.IsBound())
	{
		OnShowPreviewTriggered.Execute(Point);
	}
}

//call binded Place Trap Function
void UGridInteractComponent::PlaceTrap (FGridSurface* GridSurface)
{
	if(OnPlaceTrapTriggered.IsBound())
	{
		OnPlaceTrapTriggered.Execute(GridSurface);
	}
}
