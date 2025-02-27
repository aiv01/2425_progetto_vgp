// FABIO GIANNINO
// LUCA CASAMENTI

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
		USkeletalMeshComponent* PlayerMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
		if (PlayerMesh)
		{
			SpawnWeapons(PlayerMesh);
			EquipWeapon(0);
		}
	}
}

void UBaseWeaponComponent::EquipWeapon(int32 indexToEquip)
{
	if (indexToEquip < 0 || indexToEquip > Weapons.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("U are trying to equip a fake weapon!"));
		return;
	}
	//hide current
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(true);
	//show next
	CurrentWeaponIndex = indexToEquip;
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(false);
}

void UBaseWeaponComponent::ChangeWeapon(bool bForward)
{
	if (Weapons.Num() < 2)
	{
		return;
	}
	CurrentWeaponIndex = bForward ? CurrentWeaponIndex + 1 : CurrentWeaponIndex - 1;
	CurrentWeaponIndex = CurrentWeaponIndex % Weapons.Num();
	EquipWeapon(CurrentWeaponIndex);
}

void UBaseWeaponComponent::SpawnWeapons(USkeletalMeshComponent* PlayerMesh)
{
	if (!PlayerMesh || WeaponClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No weapons to spawn or PlayerMesh is null!"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null!"));
		return;
	}

	for (TSubclassOf<ABaseWeapon> WeaponClass : WeaponClasses)
	{
		if (WeaponClass)
		{
			ABaseWeapon* SpawnedWeapon = World->SpawnActor<ABaseWeapon>(WeaponClass);
			if (SpawnedWeapon)
			{
				Weapons.Add(SpawnedWeapon);
				SpawnedWeapon->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
				SpawnedWeapon->SetActorHiddenInGame(true);
				UE_LOG(LogTemp, Warning, TEXT("Weapon spawned, added to array and attached: %s"), *SpawnedWeapon->GetName());
			}
		}
	}
}