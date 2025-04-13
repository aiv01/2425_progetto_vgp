// Fill out your copyright notice in the Description page of Project Settings.
// Andreea Manuela Hodor

#include "AIV_2025_VGP/GOAP/Strategy/GOAPGrenadeStrategy.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AIV_2025_VGP/MainCharacter/AbilitySystem/AbilitySystemComponent.h"

void UGOAPGrenadeStrategy::Execute()
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

			if (StrategyOwner)
			{
				USkeletalMeshComponent* MeshComponent = StrategyOwner->GetMesh();
				if (MeshComponent)
				{
					MeshComponent->PlayAnimation(AnimationToPlay, true);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Animation is playing correctly");
				}
			}
		}
	}
}

