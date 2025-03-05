// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele

#pragma once

#include "CoreMinimal.h"
#include "ESurfaceOrientation.generated.h"

/**
 * 
 */

// used to let the user decided in which direction to generate surfaces for the grid system
UENUM(BlueprintType)
enum ESurfaceOrientation
{
	ZPOS		UMETA(DisplayName = "Up"),
	ZNEG		UMETA(DisplayName = "Down"),
	YPOS		UMETA(DisplayName = "Right"),
	YNEG		UMETA(DisplayName = "Left"),
	XPOS		UMETA(DisplayName = "Forward"),
	XNEG		UMETA(DisplayName = "Backward")
};
