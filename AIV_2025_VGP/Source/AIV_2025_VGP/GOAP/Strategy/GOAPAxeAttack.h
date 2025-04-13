// Fill out your copyright notice in the Description page of Project Settings.
// Andreea Manuela Hodor

#pragma once

#include "CoreMinimal.h"
#include "AIV_2025_VGP/GOAP/GOAPBaseCombatStrategy.h"
#include "GOAPAxeAttack.generated.h"

UCLASS()
class AIV_2025_VGP_API UGOAPAxeAttack : public UGOAPBaseCombatStrategy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	UAnimSequence* AnimationToPlay;

	virtual void Execute() override;
	
};
