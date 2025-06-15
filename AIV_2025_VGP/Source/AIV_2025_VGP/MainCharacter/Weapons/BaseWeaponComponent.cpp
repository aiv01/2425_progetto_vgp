// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#include "BaseWeaponComponent.h"

#include "EntitySystem/MovieSceneEntitySystemRunner.h"

UBaseWeaponComponent::UBaseWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBaseWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		PlayerMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
		if (PlayerMesh)
		{
			SpawnWeapons();
			EquipWeapon();
		}
	}
	
	World = GetWorld();
}

//Blueprint Callable Overload function with index param
void UBaseWeaponComponent::EquipWeapon(int32 IndexToEquip)
{
	if (IndexToEquip < 0 || IndexToEquip >= Weapons.Num())
	{
		return;
	}

	//Current Weapon
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(true); 
	
	//New Weapon
	CurrentWeaponIndex = IndexToEquip;
	EquipWeapon();
}

//Internal Overload function with no index param
void UBaseWeaponComponent::EquipWeapon()
{
	if (CurrentWeaponIndex < 0 || CurrentWeaponIndex >= Weapons.Num())
	{
		return;
	}

	//New Weapon
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(false);
	CurrentWeaponName = Weapons[CurrentWeaponIndex]->WeaponName;
}

void UBaseWeaponComponent::ChangeWeapon(bool bForward)
{
	if (Weapons.Num() < 2)
	{
		return;
	}

	//Hide current weapon
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(true);
	//Get next weapon
	CurrentWeaponIndex = bForward ? CurrentWeaponIndex + 1 : CurrentWeaponIndex - 1;
	
	if (CurrentWeaponIndex < 0)
	{
		CurrentWeaponIndex = Weapons.Num() - 1;
	}
	else
	{
		CurrentWeaponIndex = CurrentWeaponIndex % (Weapons.Num());
	}
	//Call overload with no param
	EquipWeapon();
}

void UBaseWeaponComponent::AddNewWeapon(TSubclassOf<ABaseWeapon> NewWeapon)
{
	if (!World)
	{
		World = GetWorld();
		if (!World)
		{
			return;
		}
	}
	
	ABaseWeapon* SpawnedWeapon = World->SpawnActor<ABaseWeapon>(NewWeapon);
	if (SpawnedWeapon && PlayerMesh)
	{
		Weapons.Add(SpawnedWeapon);
		if (SpawnedWeapon->WeaponName == "Rifle")
		{
			SpawnedWeapon->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_l"));
		}
		else
		{
			SpawnedWeapon->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		}
		SpawnedWeapon->SetOwner(GetOwner());
		SpawnedWeapon->SetActorHiddenInGame(true);
	}
}

void UBaseWeaponComponent::SpawnWeapons()
{
	if (!PlayerMesh || WeaponClasses.Num() == 0)
	{
		return;
	}
	for (TSubclassOf<ABaseWeapon> WeaponClass : WeaponClasses)
	{
		AddNewWeapon(WeaponClass);
	}
}

void UBaseWeaponComponent::StartPrimaryAttack()
{
	if(Weapons[CurrentWeaponIndex])
	{
		Weapons[CurrentWeaponIndex]->PrimaryAttackEventStart();
	}
}

void UBaseWeaponComponent::EndPrimaryAttack()
{
	if(Weapons[CurrentWeaponIndex])
	{
		Weapons[CurrentWeaponIndex]->PrimaryAttackEventEnd();
	}
}

void UBaseWeaponComponent::StartSecondaryAttack()
{
	if(Weapons[CurrentWeaponIndex])
	{
		Weapons[CurrentWeaponIndex]->SecondaryAttackEventStart();
	}
}

void UBaseWeaponComponent::EndSecondaryAttack()
{
	if(Weapons[CurrentWeaponIndex])
	{
		Weapons[CurrentWeaponIndex]->SecondaryAttackEventEnd();
	}
}

void UBaseWeaponComponent::StartAnimationCurrentWeapon()
{
	if(Weapons[CurrentWeaponIndex])
	{
		Weapons[CurrentWeaponIndex]->WeaponAnimationStart();
	}
}
#pragma region Getter
void UBaseWeaponComponent::GetPrimaryAttackName(FName& PrimaryAttackName)
{
	PrimaryAttackName = Weapons[CurrentWeaponIndex]->PrimaryAttackName;
}

void UBaseWeaponComponent::GetSecondaryAttackName(FName& SecondaryAttackName)
{
	SecondaryAttackName = Weapons[CurrentWeaponIndex]->SecondaryAttackName;
}
#pragma endregion	
