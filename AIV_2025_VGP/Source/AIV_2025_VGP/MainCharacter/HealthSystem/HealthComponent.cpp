// Copyright Epic Games, Inc. All Rights Reserved.
// Lorenzo Petrillo
// Fabrizio Conni

#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;// To false since its not needed
	CurrentHealth = MaxHealth; // Sets current health to the maximum amount when built
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

bool UHealthComponent::SetHealth(float Amount)
{
	if (Amount >= 0 && Amount <= MaxHealth)
	{
		float prevHealth = CurrentHealth; // Store for check if is revived
		CurrentHealth = Amount;
		//Revive condition
		if(prevHealth == 0 && CurrentHealth > 0)
		{
			OnReviveCallback.Broadcast();
			return true;
		}
		//Death condition
		if(prevHealth > 0 && CurrentHealth == 0)
		{
			OnDeathCallback.Broadcast();
			return true;
		}
	}
	return false;
}

bool UHealthComponent::AddHealth(float Amount)
{
	if (Amount > 0 && CurrentHealth > 0) // Checks if amount is more then 0 and its needed to do the elaborate the clamp
	{
		CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth); // Adds Amount to current health with a clamp
		CheckIfDeath();
		return true;
	}
	return false;
}

bool UHealthComponent:: RemoveHealth(float Amount)
{
	if (Amount > 0 && CurrentHealth > 0) // Checks if amount is more then 0 and its needed to do the elaborate the clamp
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.f, MaxHealth); // Adds Amount to current health with a clamp
		CheckIfDeath();
		return true;
	}
	return false;
}

void UHealthComponent::CheckIfDeath() const
{
	if (CurrentHealth <= 0)
	{
		OnDeathCallback.Broadcast();
	}
}
