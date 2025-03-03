// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_PlayerInput.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UI_PlayerInput : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AIV_2025_VGP_API II_PlayerInput
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Input")
	void Move(const FVector2D& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Input")
	void Look(const FVector2D& Value);
};
