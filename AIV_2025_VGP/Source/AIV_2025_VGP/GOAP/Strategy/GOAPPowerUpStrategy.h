// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIV_2025_VGP/GOAP/GOAPBaseCombatStrategy.h"
#include "GOAPPowerUpStrategy.generated.h"

/**
 * 
 */
UCLASS()
class AIV_2025_VGP_API UGOAPPowerUpStrategy : public UGOAPBaseCombatStrategy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	UAnimMontage* AnimationToPlay;

	virtual void Execute() override;
	
};
