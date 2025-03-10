// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCombatStrategy.h"
#include "PC_MeleeStrategy.generated.h"

/**
 * 
 */
UCLASS()
class AIV_2025_VGP_API UPC_MeleeStrategy : public UBaseCombatStrategy
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	UAnimMontage* MontageToPlay;
	
	virtual void Execute() override;
};
