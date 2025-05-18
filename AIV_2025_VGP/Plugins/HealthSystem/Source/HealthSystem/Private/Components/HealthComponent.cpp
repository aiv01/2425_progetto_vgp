// Fabrizio Conni

#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "FunctionLibrary/HealthSystemFunctions.h"

UHealthComponent::UHealthComponent()
{
	// This component won't tick every frame (better performance unless needed)
	PrimaryComponentTick.bCanEverTick = false;

	// Default maximum health value
	MaxHealth = 100.f;
	
	// Enables replication for the component
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize current health to maximum
	Health = MaxHealth;

	// Bind the OnChangeHealth delegate to a callback function
	OnChangeHealth.AddDynamic(this, &UHealthComponent::OnChangeHealthCallback);

	// If the actor has server authority
	if (GetOwner()->HasAuthority())
	{
		// Bind OnLanded callback based on whether revival is allowed
		if (bCanRevive)
		{
			OnLanded.AddDynamic(this, &UHealthComponent::OnLandedCallback);
		}
		else
		{
			OnLanded.AddDynamic(this, &UHealthComponent::OnDeathCallback);
		}
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, Health); // Replicate Health variable
	DOREPLIFETIME(UHealthComponent, bCanReviveFriend);
	//DOREPLIFETIME(UHealthComponent, ActorToRevive);
}

#pragma region Health System

void UHealthComponent::ChangeHealth(float Delta)
{
	if (GetOwner()->HasAuthority())
	{
		// Server has authority, apply damage immediately
		ServerChangeHealth(Delta);
	}
	else
	{
		// Client-side call for server authority to change health
		UE_LOG(LogTemp, Warning, TEXT("Client: %s, HasNoAutority"), *GetOwner()->GetName());
		ServerRPC_ChangeHealth(Delta);
	}
}

void UHealthComponent::ServerRPC_ChangeHealth_Implementation(float Delta)
{
	// Validates server authority again
	UE_LOG(LogTemp, Warning, TEXT("Server RPC Called"));
	// if (GetOwner()->HasAuthority())
	// {
		ServerChangeHealth(Delta);
	// }
}

void UHealthComponent::ServerChangeHealth(float Delta)
{
	const float OldHealth = Health;

	// Clamp health between 0 and max
	Health = FMath::Clamp(Health + Delta, 0.f, MaxHealth);

	// Manually call the replication callback with delta
	OnRep_Health(Delta);
}

void UHealthComponent::OnRep_Health(float OldHealth)
{
	float Delta = Health - OldHealth;

	UE_LOG(LogTemp, Warning, TEXT("Health Changed: %f, Current Health: %f"), Delta, Health);

	// Notify other systems of the change
	OnChangeHealth.Broadcast(GetOwner());

	// If health dropped to 0, notify landing event (which triggers death or revive)
	if (Health <= 0.f)
	{
		OnLanded.Broadcast();
	}
}

float UHealthComponent::GetPercentHealth() const
{
	// Returns health as a percentage of max health
	return Health / MaxHealth;
}

void UHealthComponent::MakeDamageToActor(float Damage, AActor* TargetActor, AActor* Instigator)
{
	if(Damage <= 0 || TargetActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Error To Use Method"));
		return;
	}

	if(GetOwner()->HasAuthority())
	{
		UHealthSystemFunctions::MakeDamage_Internal(Damage, TargetActor);
	}
	else
	{
		// Client-side call for server authority to apply damage
		ServerRPC_MakeDamageToActor(Damage, TargetActor, Instigator);
	}
}

void UHealthComponent::ServerRPC_MakeDamageToActor_Implementation(float Damage, AActor* TargetActor, AActor* Instigator)
{
	UHealthSystemFunctions::MakeDamage_Internal(Damage, TargetActor);
}

void UHealthComponent::HealToActor(float Heal, AActor* TargetActor, AActor* Instigator)
{
	if(Heal <= 0 || TargetActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Error To Use Method"));
		return;
	}

	if(GetOwner()->HasAuthority())
	{
		UHealthSystemFunctions::Healing_Internal(Heal, TargetActor);
	}
	else
	{
		// Client-side call for server authority to apply damage
		ServerRPC_HealToActor(Heal, TargetActor, Instigator);
	}
}

void UHealthComponent::ServerRPC_HealToActor_Implementation(float Heal, AActor* TargetActor, AActor* Instigator)
{
	UHealthSystemFunctions::Healing_Internal(Heal, TargetActor);
}
#pragma endregion

#pragma region Callbacks
void UHealthComponent::OnLandedCallback()
{
    // Visual log for debugging
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
        FString::Printf(TEXT("Player %s is Landed"), *GetOwner()->GetName()));

    // Delay death callback after a timer (used if revivable)
    GetWorld()->GetTimerManager().SetTimer(
        DeathTimerHandle, this, &UHealthComponent::OnDeathCallback, LandedTimer, false);
}

void UHealthComponent::OnDeathCallback()
{
    // Death visual feedback
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
        FString::Printf(TEXT("Player %s is Dead"), *GetOwner()->GetName()));
}

void UHealthComponent::OnChangeHealthCallback(AActor* Actor)
{
    // Placeholder: Can be expanded to trigger animations, UI changes, etc.
}
#pragma endregion

#pragma region Revive System
void UHealthComponent::ChangeReviveFriendStatus(AActor* Target, bool bNewStatus, AActor* SelfRef)
{
	if (GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client: %s, HasAutority"), *GetOwner()->GetName());
		// Server has authority, apply damage immediately
		UHealthSystemFunctions::FriendCanRevive_Internal(Target, bNewStatus, SelfRef);
	}
	else
	{
		// Client-side call for server authority to change health
		UE_LOG(LogTemp, Warning, TEXT("Client: %s, HasNoAutority"), *GetOwner()->GetName());
		ServerRPC_ChangeReviveFriendStatus(Target, bNewStatus, SelfRef);
	}
}

void UHealthComponent::ServerRPC_ChangeReviveFriendStatus_Implementation(AActor* Target, bool bNewStatus, AActor* SelfRef)
{
	UHealthSystemFunctions::FriendCanRevive_Internal(Target, bNewStatus, SelfRef);
}

void UHealthComponent::StopLandedTimer()
{
	DeathTimerHandle.Invalidate();
}
#pragma endregion