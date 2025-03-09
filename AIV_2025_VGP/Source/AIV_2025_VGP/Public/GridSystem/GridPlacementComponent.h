// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FGridSurface.h"
#include "Components/ActorComponent.h"
#include "GridPlacementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIV_2025_VGP_API UGridPlacementComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	UGridPlacementComponent();
	
	void PlaceTrap(FGridSurface* GridSurface);
	
};
