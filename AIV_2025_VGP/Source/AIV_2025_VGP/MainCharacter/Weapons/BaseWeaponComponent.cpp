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

//Blueprint Callable Overload function with index param
void UBaseWeaponComponent::EquipWeapon(int32 indexToEquip)
{
	UE_LOG(LogTemp, Log, TEXT("EquipWeapon - Index: %d"), indexToEquip);
	if (indexToEquip < 0 || indexToEquip >= Weapons.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("U are trying to equip a fake weapon!"));
		return;
	}

	//Current Weapon
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(true); 
	
	//New Weapon
	CurrentWeaponIndex = indexToEquip;
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(false);
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

	//Get next weapon
	CurrentWeaponIndex = bForward ? CurrentWeaponIndex + 1 : CurrentWeaponIndex - 1;

	// Assicura che l'indice sia circolare anche per valori negativi
	if (CurrentWeaponIndex < 0)
	{
		CurrentWeaponIndex = Weapons.Num() - 1;
	}
	else
	{
		CurrentWeaponIndex = CurrentWeaponIndex % Weapons.Num();
	}

	//Call overload with no param
	EquipWeapon();
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