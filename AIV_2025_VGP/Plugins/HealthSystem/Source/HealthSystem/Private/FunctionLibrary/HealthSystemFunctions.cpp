// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/HealthSystemFunctions.h"
#include "Interfaces/I_HealthInterface.h"

void UHealthSystemFunctions::MakeDamage(float Damage, AActor* TargetActor)
{
	if(Damage <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Damage Must Be Positive"));
		return;
	}
	
	Damage *= -1;
	
	if(TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Called"));
		II_HealthInterface* HealthInterface = Cast<II_HealthInterface>(TargetActor);
		if(HealthInterface)
		{
			II_HealthInterface::Execute_I_SetHealth(TargetActor, Damage);
		}
	}
}

void UHealthSystemFunctions::Healing(float Heal, AActor* TargetActor)
{
	if(Heal <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Heal Must Be Positive"));
		return;
	}
	
	if(TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Called"));
		II_HealthInterface* HealthInterface = Cast<II_HealthInterface>(TargetActor);
		if(HealthInterface)
		{
			II_HealthInterface::Execute_I_SetHealth(TargetActor, Heal);
		}
	}
}
