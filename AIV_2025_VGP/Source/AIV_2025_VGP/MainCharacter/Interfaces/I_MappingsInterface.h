// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_MappingsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UI_MappingsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class AIV_2025_VGP_API II_MappingsInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// function to go from combat to trap mode
	UFUNCTION(BlueprintCallable)
	virtual bool SwitchToTrapMode();

	// function to go from trap to combat mode
	UFUNCTION(BlueprintCallable)
	virtual bool SwitchToCombatMode();
};

