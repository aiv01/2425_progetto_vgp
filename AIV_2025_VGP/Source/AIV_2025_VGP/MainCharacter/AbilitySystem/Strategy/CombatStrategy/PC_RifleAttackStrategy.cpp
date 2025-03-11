// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_RifleAttackStrategy.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void UPC_RifleAttackStrategy::Execute()
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