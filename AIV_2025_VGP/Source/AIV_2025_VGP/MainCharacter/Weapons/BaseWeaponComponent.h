// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#pragma once

#include "CoreMinimal.h"
#include "BaseRangedWeapon.h"
#include "BaseWeapon.h"
#include "Components/ActorComponent.h"
#include "BaseWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIV_2025_VGP_API UBaseWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#pragma region Properties
private:
	UPROPERTY(VisibleAnywhere)
	TArray<ABaseWeapon*> Weapons;
	int32 CurrentWeaponIndex = 0;	
	USkeletalMeshComponent* PlayerMesh;
	UWorld* World;
			
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Weapon")
	TArray<TSubclassOf<ABaseWeapon>> WeaponClasses;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Weapon")
	TMap<TSubclassOf<ABaseRangedWeapon>, int32> RangedWeaponAmmoMap;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="Weapon")
	FName CurrentWeaponName = FName("");
#pragma endregion Properties

	
#pragma region Functions
private:
	void EquipWeapon();
	void SpawnWeapons();
	
public:
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(int32 IndexToEquip);
	
	UFUNCTION(BlueprintCallable)
	void ChangeWeapon(bool bForward);
	
	UFUNCTION(BlueprintCallable)
	void AddNewWeapon(TSubclassOf<ABaseWeapon> NewWeapon);

	UFUNCTION(BlueprintCallable)
	void StartPrimaryAttack();
	UFUNCTION(BlueprintCallable)
	void EndPrimaryAttack();
	UFUNCTION(BlueprintCallable)
	void StartSecondaryAttack();
	UFUNCTION(BlueprintCallable)
	void EndSecondaryAttack();
	UFUNCTION(BlueprintCallable)
	void StartAnimationCurrentWeapon();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CurrentWeapon")
	void GetPrimaryAttackName(FName& PrimaryAttackName);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CurrentWeapon")
	void GetSecondaryAttackName(FName& SecondaryAttackName);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CurrentWeapon")
	bool CanAttack() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Ranged Weapons")
	bool CanReload() const;
	UFUNCTION(BlueprintCallable, Category="Ranged Weapons")
	void Reload();
	UFUNCTION(BlueprintCallable, Category="Ranged Weapons")
	bool AddAmmo (TSubclassOf<ABaseRangedWeapon>  WeaponClass, int32 Amount);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	ABaseWeapon* GetCurrentWeapon();

#pragma endregion Functions
};
