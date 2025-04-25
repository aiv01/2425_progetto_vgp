// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele

#pragma once

#include "CoreMinimal.h"
#include "ETrapType.generated.h"

/**
 * 
 */

#pragma once

UENUM(BlueprintType)
enum ETrapType : uint8
{
	Wall		UMETA(DisplayName = "Wall"),
	Floor		UMETA(DisplayName = "Floor"),
	Turret		UMETA(DisplayName = "Turret")
};