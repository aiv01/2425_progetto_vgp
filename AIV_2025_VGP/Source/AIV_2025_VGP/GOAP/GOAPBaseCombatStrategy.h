// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIV_2025_VGP/GOAP/Strategy/GOAPBaseStrategy.h"
#include "GOAPBaseCombatStrategy.generated.h"

/**
 * 
 */
UCLASS()
class AIV_2025_VGP_API UGOAPBaseCombatStrategy : public UGOAPBaseStrategy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float Cooldown = 0.0f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	UAnimMontage* MontageToPlay;

	float LastTimeUse = -FLT_MAX;
	bool CanAttack();
	virtual void Execute() override;
};
