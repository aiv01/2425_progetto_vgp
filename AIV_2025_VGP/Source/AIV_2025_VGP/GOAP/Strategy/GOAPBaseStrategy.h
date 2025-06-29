// Fill out your copyright notice in the Description page of Project Settings.
// Andreea Manuela Hodor

#pragma once

#include "CoreMinimal.h"
#include "AIV_2025_VGP/MainCharacter/AbilitySystem/Strategy/BaseStrategy.h"
#include "GOAPBaseStrategy.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew)
class AIV_2025_VGP_API UGOAPBaseStrategy : public UBaseStrategy
{
	GENERATED_BODY()

public:
	
	virtual void Execute() override;

	
};
