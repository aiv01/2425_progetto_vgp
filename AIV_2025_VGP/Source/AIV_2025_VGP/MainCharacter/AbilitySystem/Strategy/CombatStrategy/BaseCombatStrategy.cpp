// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

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

void UBaseCombatStrategy::Execute(AActor* Executor)
{
}