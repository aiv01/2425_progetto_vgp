// Fill out your copyright notice in the Description page of Project Settings.
// Andreea Manuela Hodor

#include "AIV_2025_VGP/GOAP/Strategy/GOAPPowerUpStrategy.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AIV_2025_VGP/MainCharacter/AbilitySystem/AbilitySystemComponent.h"

void UGOAPPowerUpStrategy::Execute()
{
	if (CanAttack())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, "MeleeStrategy");
		}

		if (AnimationToPlay)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Melee atk!!!");

			StrategyOwner->PlayAnimMontage(AnimationToPlay);
		}
	}
}