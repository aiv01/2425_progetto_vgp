// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#include "BaseWeaponComponent.h"

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
	UE_LOG(LogTemp, Log, TEXT("EquipWeapon - Index: %d"), IndexToEquip);
	if (IndexToEquip < 0 || IndexToEquip >= Weapons.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("U are trying to equip a fake weapon!"));
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
	UE_LOG(LogTemp, Log, TEXT("EquipWeapon - Index: %d"), CurrentWeaponIndex);
	if (CurrentWeaponIndex < 0 || CurrentWeaponIndex >= Weapons.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("U are trying to equip a fake weapon!"));
		return;
	}

	//New Weapon
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(false);
	CurrentWeaponName = Weapons[CurrentWeaponIndex]->WeaponName;
}

void UBaseWeaponComponent::ChangeWeapon(bool bForward)
{
	UE_LOG(LogTemp, Log, TEXT("ChangeWeapon"));
	if (Weapons.Num() < 2)
	{
		return;
	}

	//Hide current weapon
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(true);
	UE_LOG(LogTemp, Log, TEXT("EquipWeapon - Index: %d"), CurrentWeaponIndex);
	//Get next weapon
	CurrentWeaponIndex = bForward ? CurrentWeaponIndex + 1 : CurrentWeaponIndex - 1;
	UE_LOG(LogTemp, Log, TEXT("EquipWeapon - Index: %d"), CurrentWeaponIndex);
	// Assicura che l'indice sia circolare anche per valori negativi
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
			UE_LOG(LogTemp, Error, TEXT("World is null!"));
			return;
		}
	}
	
	ABaseWeapon* SpawnedWeapon = World->SpawnActor<ABaseWeapon>(NewWeapon);
	if (SpawnedWeapon && PlayerMesh)
	{
		Weapons.Add(SpawnedWeapon);
		SpawnedWeapon->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		SpawnedWeapon->SetActorHiddenInGame(true);
		UE_LOG(LogTemp, Warning, TEXT("Weapon spawned, added to array and attached: %s"), *SpawnedWeapon->GetName());
	}
}

void UBaseWeaponComponent::SpawnWeapons()
{
	if (!PlayerMesh || WeaponClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No weapons to spawn or PlayerMesh is null!"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Weapon spawned, added to array and attached: %d"), WeaponClasses.Num());
	for (TSubclassOf<ABaseWeapon> WeaponClass : WeaponClasses)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon spawned, added to array and attached: %s"), *WeaponClass->GetName());
		AddNewWeapon(WeaponClass);
	}
}

void UBaseWeaponComponent::StartAttackWithCurrentWeapon()
{
	if(Weapons[CurrentWeaponIndex])
	{
		Weapons[CurrentWeaponIndex]->AttackEventStart();
	}
}

void UBaseWeaponComponent::EndAttackWithCurrentWeapon()
{
	if(Weapons[CurrentWeaponIndex])
	{
		Weapons[CurrentWeaponIndex]->AttackEventEnd();
	}
}

void UBaseWeaponComponent::StartAnimationCurrentWeapon()
{
	if(Weapons[CurrentWeaponIndex])
	{
		Weapons[CurrentWeaponIndex]->WeaponAnimationStart();
	}
}
