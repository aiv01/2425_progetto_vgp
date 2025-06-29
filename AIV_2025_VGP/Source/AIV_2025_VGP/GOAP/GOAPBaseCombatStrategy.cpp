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
	if (CanAttack())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, "MeleeStrategy");
		}

		if (MontageToPlay)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Melee atk!!!");

			if (StrategyOwner)
			{
				StrategyOwner->PlayAnimMontage(MontageToPlay);
			}

		}
	}
}