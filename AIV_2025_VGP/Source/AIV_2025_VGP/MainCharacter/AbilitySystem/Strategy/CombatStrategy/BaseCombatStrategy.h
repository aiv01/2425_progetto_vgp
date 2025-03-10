// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseStrategy.h"
#include "BaseCombatStrategy.generated.h"

/**
 * 
 */
UCLASS()
class AIV_2025_VGP_API UBaseCombatStrategy : public UBaseStrategy
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float Cooldown = 0.0f;

protected:
	float LastTimeUse = -FLT_MAX;
	bool CanAttack();
	virtual void Execute() override;
};
