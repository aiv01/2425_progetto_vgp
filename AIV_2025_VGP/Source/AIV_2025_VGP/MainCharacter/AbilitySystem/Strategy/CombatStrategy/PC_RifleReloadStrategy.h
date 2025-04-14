// Fill out your copyright notice in the Description page of Project Settings.
//Fabio Giannino
#pragma once

#include "CoreMinimal.h"
#include "BaseCombatStrategy.h"
#include "PC_RifleReloadStrategy.generated.h"

/**
 * 
 */
UCLASS()
class AIV_2025_VGP_API UPC_RifleReloadStrategy : public UBaseCombatStrategy
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	UAnimMontage* MontageToPlay;
	
	virtual void Execute() override;
};
