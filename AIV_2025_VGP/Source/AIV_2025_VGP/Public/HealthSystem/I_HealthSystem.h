// Copyright Epic Games, Inc. All Rights Reserved.
// Lorenzo Petrillo
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_HealthSystem.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UI_HealthSystem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AIV_2025_VGP_API II_HealthSystem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	void IAddHealth(float Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	void IRemoveHealth(float Amount);
};
