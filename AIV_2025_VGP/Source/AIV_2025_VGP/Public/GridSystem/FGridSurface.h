// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FGridSurface.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct AIV_2025_VGP_API FGridSurface
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (TitleProperty = "Position"))
	FVector Position;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (TitleProperty = "Orientation"))
	FVector Orientation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (TitleProperty = "Occupied"))
	bool bOccupied;
};
