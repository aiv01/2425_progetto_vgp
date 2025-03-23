// Fill out your copyright notice in the Description page of Project Settings.
//Andreea Hodor

#include "GOAPMelee.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UGOAPMelee::Execute()
{
	if (CanAttack())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "MeleeStrategy");
		}

		if (MontageToPlay)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "This is a temp strategy");
			/*ACharacter* GOAPAgent = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			if (GOAPAgent)
			{
				GOAPAgent->PlayAnimMontage(MontageToPlay);
			}*/
		}
	}
}
