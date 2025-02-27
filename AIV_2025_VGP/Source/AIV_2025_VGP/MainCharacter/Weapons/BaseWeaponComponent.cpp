//FABIO GIANNINO


#include "BaseWeaponComponent.h"

// Sets default values for this component's properties
UBaseWeaponComponent::UBaseWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}


// Called when the game starts
void UBaseWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();  // Otteniamo il player che possiede questo componente
	if (Owner && WeaponClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			// Spawn dell'arma dall'Inspector
			CurrentWeapon = World->SpawnActor<ABaseWeapon>(WeaponClass);

			if (CurrentWeapon)
			{
				USkeletalMeshComponent* PlayerMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
				if (PlayerMesh)
				{
					EquipWeapon(CurrentWeapon, PlayerMesh);
				}
			}
		}
	}
}


void UBaseWeaponComponent::EquipWeapon(ABaseWeapon* NewWeapon, USkeletalMeshComponent* PlayerMesh)
{
	if (NewWeapon && PlayerMesh)
	{
		CurrentWeapon = NewWeapon;
		CurrentWeapon->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
	}
}

void UBaseWeaponComponent::ChangeWeapon()
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (World)
	{
		// Spawn dell'arma dall'Inspector
		CurrentWeapon = World->SpawnActor<ABaseWeapon>(SecondaryWeaponClass);

		if (CurrentWeapon)
		{
			USkeletalMeshComponent* PlayerMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
			if (PlayerMesh)
			{
				EquipWeapon(CurrentWeapon, PlayerMesh);
			}
		}
	}
}
