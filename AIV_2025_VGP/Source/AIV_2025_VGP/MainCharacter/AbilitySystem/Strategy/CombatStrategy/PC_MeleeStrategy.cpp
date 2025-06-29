// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#include "PC_MeleeStrategy.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UPC_MeleeStrategy::Execute(AActor* Executor)
{
	if (CanAttack())
	{ 
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "MeleeStrategy");
		}

		if (MontageToPlay)
		{
			ACharacter* PlayerCharacter = Cast<ACharacter>(Executor);
			if(PlayerCharacter)
			{
				if (PlayerCharacter->HasAuthority())
				{
					Multicast_SyncAnim_Implementation(PlayerCharacter);
				}
				else
				{
					Server_SyncAnim_Implementation(PlayerCharacter);
				}
								
			
			}
		}
	}
}

void UPC_MeleeStrategy::Server_SyncAnim_Implementation(ACharacter* owner)
{
	Multicast_SyncAnim_Implementation(owner);
}

void UPC_MeleeStrategy::Multicast_SyncAnim_Implementation(ACharacter* owner)
{
	owner->PlayAnimMontage(MontageToPlay);
}
