// Copyright Epic Games, Inc. All Rights Reserved.
// Lorenzo Petrillo


#include "HealthSystem/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;// Setted to false since its not needed
	CurrentHealth = MaxHealth; // Sets current health to the maximum amount when built

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UHealthComponent::AddHealth(float Amount)
{
	if (Amount > 0) // Checks if amount is more then 0 and its needed to do the elaborate the clamp
	{
		CurrentHealth = FMath::Clamp(Amount, 0.f, MaxHealth); // Adds Amount to current health trought a clamp
	}
}


void UHealthComponent:: RemoveHealth(float Amount)
{
	if (Amount > 0) // Checks if amount is more then 0 and its needed to do the elaborate the clamp
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.f, MaxHealth); // Adds Amount to current health trought a clamp
	}
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

