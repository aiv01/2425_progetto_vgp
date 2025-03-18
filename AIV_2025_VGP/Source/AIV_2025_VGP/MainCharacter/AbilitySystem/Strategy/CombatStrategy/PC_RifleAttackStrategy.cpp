// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#include "PC_RifleAttackStrategy.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void UPC_RifleAttackStrategy::Execute()
{
	if (CanAttack())
	{ 
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "RangedStrategy");
		}

		if (MontageToPlay)
		{
			ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			if(PlayerCharacter)
			{
				PlayerCharacter->PlayAnimMontage(MontageToPlay);				
			}
		}
	}
}