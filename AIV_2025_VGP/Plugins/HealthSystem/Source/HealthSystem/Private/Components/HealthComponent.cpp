// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	MaxHealth = 100.f;		
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicatedByDefault(true);
	Health = MaxHealth;
	OnChangeHealth.AddDynamic(this, &UHealthComponent::OnChangeHealthCallback);
	if(GetOwner()->HasAuthority())
	{		
		if(bCanRevive)
		{
			OnLanded.AddDynamic(this, &UHealthComponent::OnLandedCallback);
		}
		else
		{
			OnLanded.AddDynamic(this, &UHealthComponent::OnDeathCallback);
		}
	}
	
}

float UHealthComponent::GetPercentHealth() const
{
	return Health / MaxHealth;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, Health);
}

void UHealthComponent::ChangeHealth(float Delta)
{
	if(GetOwner()->HasAuthority())
	{
		ServerChangeHealth(Delta);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Client: %s, HasNoAutority"), *GetOwner()->GetName());
		ServerRPC_ChangeHealth(Delta);
	}
}

void UHealthComponent::ServerRPC_ChangeHealth_Implementation(float Delta)
{
	UE_LOG(LogTemp, Warning, TEXT("Server RPC Called"));
	if(GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server Has Authority Call Server Danage"));
		ServerChangeHealth(Delta);
	}
}

void UHealthComponent::ServerChangeHealth(float Delta)
{
	const float OldHealth = Health;
	Health = FMath::Clamp(Health + Delta, 0.f, MaxHealth);
	OnRep_Health(Delta);
}

void UHealthComponent::OnRep_Health(float OldHealth)
{
	float Delta = Health - OldHealth;
	UE_LOG(LogTemp, Warning, TEXT("Health Changed: %f, Current Health: %f"), Delta, Health);
	OnChangeHealth.Broadcast(GetOwner());
	if(Health <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnLandedBroadcast Called"));
		OnLanded.Broadcast();
	}
}

#pragma region Callbacks
void UHealthComponent::OnLandedCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("OnLandedCallback Called"));
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Orange,
		FString::Printf(TEXT("Player %s is Landed"), *GetOwner()->GetName())
	);
	
	GetWorld()->GetTimerManager().SetTimer(
		DeathTimerHandle,
		this,
		&UHealthComponent::OnDeathCallback,
		LandedTimer,
		false
		);
}

void UHealthComponent::OnDeathCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("OnDeathCallback Called"));

	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Red,
		FString::Printf(TEXT("Player %s is Dead"), *GetOwner()->GetName())
	);
}

void UHealthComponent::OnChangeHealthCallback(AActor* Actor)
{
}
#pragma endregion Callbacks

