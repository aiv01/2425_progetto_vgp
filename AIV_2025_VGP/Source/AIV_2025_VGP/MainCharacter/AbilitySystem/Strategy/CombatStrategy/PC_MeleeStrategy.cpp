// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#include "PC_MeleeStrategy.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UPC_MeleeStrategy::Execute()
{
	if (CanAttack())
	{ 
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "MeleeStrategy");
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
