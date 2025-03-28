// Fill out your copyright notice in the Description page of Project Settings.


#include "AIV_2025_VGP/GOAP/GOAPBaseCombatStrategy.h"

bool UGOAPBaseCombatStrategy::CanAttack()
{
	if (GetWorld()->GetTimeSeconds() - LastTimeUse >= Cooldown)
	{
		LastTimeUse = GetWorld()->GetTimeSeconds();
		return true;
	}

	return false;
}

void UGOAPBaseCombatStrategy::Execute()
{
}