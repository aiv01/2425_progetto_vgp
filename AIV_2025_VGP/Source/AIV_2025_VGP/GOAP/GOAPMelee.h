// Fill out your copyright notice in the Description page of Project Settings.
//Andreea Hodor

#pragma once

#include "CoreMinimal.h"
#include "AIV_2025_VGP/MainCharacter/AbilitySystem/Strategy/CombatStrategy/BaseCombatStrategy.h"
#include "GOAPMelee.generated.h"

/**
 * 
 */
UCLASS()
class AIV_2025_VGP_API UGOAPMelee : public UBaseCombatStrategy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	UAnimMontage* MontageToPlay;

	virtual void Execute() override;
	
};
