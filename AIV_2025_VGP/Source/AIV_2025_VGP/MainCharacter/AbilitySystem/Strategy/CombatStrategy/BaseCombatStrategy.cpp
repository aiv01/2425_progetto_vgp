// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCombatStrategy.h"

bool UBaseCombatStrategy::CanAttack()
{
	if(GetWorld()->GetTimeSeconds() - LastTimeUse >= Cooldown)
	{
		LastTimeUse = GetWorld()->GetTimeSeconds();
		return true;
	}

	return false;
}

void UBaseCombatStrategy::Execute()
{
}