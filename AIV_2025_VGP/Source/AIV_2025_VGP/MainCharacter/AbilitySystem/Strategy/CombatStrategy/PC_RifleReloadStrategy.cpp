// Fill out your copyright notice in the Description page of Project Settings.
//Fabio Giannino

#include "PC_RifleReloadStrategy.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UPC_RifleReloadStrategy::Execute()
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