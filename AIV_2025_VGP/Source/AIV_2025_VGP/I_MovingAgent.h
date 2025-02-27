// Fill out your copyright notice in the Description page of Project Settings.
// Marco Pungillo
// Andreea Hodor
// Alessandro Violante

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_MovingAgent.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_MovingAgent : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AIV_2025_VGP_API II_MovingAgent
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Getters")
	float I_GetSpeed();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Setters")
	void I_SetSpeed(const float NewSpeed);

};
