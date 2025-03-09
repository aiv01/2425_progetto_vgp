// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem/GridProcessorComponent.h"

#include "GridSystem/GridGeneratorVolume.h"

void UGridProcessorComponent::InitializeComponent ()
{
	//check if the owner is from "GridGeneratorVolume" class
	if( GetOwner()->IsA(AGridGeneratorVolume::StaticClass()))
	{
		//link ref
		AGridGeneratorVolume* owner = Cast<AGridGeneratorVolume>(GetOwner());
		TArray<FGridSurface> Gd = owner->GetGridData();
		GridData = &Gd;
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner is not a 'AGridGeneratorVolume'"));
	}
}
