// Fabrizio Conni

#include "FunctionLibrary/HealthSystemFunctions.h"
#include "Interfaces/I_HealthInterface.h"

void UHealthSystemFunctions::MakeDamage(float Damage, AActor* TargetActor)
{
	MakeDamage_Internal(Damage, TargetActor);
}

void UHealthSystemFunctions::MakeDamage_Internal(float Damage, AActor* TargetActor)
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
		if (TargetActor->GetClass()->ImplementsInterface(UI_HealthInterface::StaticClass()))
		{
			II_HealthInterface::Execute_I_SetHealth(TargetActor, Damage);
		}
	}
}

void UHealthSystemFunctions::Healing(float Heal, AActor* TargetActor)
{
	Healing_Internal(Heal, TargetActor);
}

void UHealthSystemFunctions::Healing_Internal(float Heal, AActor* TargetActor)
{
	if(Heal <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Heal Must Be Positive"));
		return;
	}
	
	if(TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Called"));
		if (TargetActor->GetClass()->ImplementsInterface(UI_HealthInterface::StaticClass()))
		{
			II_HealthInterface::Execute_I_SetHealth(TargetActor, Heal);
		}
	}
}

void UHealthSystemFunctions::FriendCanRevive_Internal(AActor* TargetActor, bool bNewStatus, AActor* SelfRef)
{
	if(TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Called"));
		if (TargetActor->GetClass()->ImplementsInterface(UI_HealthInterface::StaticClass()))
		{
			II_HealthInterface::Execute_I_FriendCanRevive(TargetActor, bNewStatus, SelfRef);
		}
	}
}
