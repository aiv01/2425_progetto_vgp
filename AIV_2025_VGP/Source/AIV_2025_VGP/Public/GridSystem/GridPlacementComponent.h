// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele
#pragma once

#include "CoreMinimal.h"
#include "FGridSurface.h"
#include "GridProcessorComponent.h"
#include "Components/ActorComponent.h"
#include "GridPlacementComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class AIV_2025_VGP_API UGridPlacementComponent : public UGridProcessorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	UGridPlacementComponent();
	void PlaceTrap(const FHitResult HitResult);
	
};
