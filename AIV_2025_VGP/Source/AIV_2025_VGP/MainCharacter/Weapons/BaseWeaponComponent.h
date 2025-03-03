// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#pragma once

#include "CoreMinimal.h"
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
	TArray<ABaseWeapon*> Weapons;
	int32 CurrentWeaponIndex = 0;	
	USkeletalMeshComponent* PlayerMesh;
	UWorld* World;
	
public:
	UPROPERTY(EditAnywhere, Category="Weapon")
	TArray<TSubclassOf<ABaseWeapon>> WeaponClasses;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="Weapon")
	FString CurrentWeaponName = FString("");
	
#pragma endregion

	
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
	void AttackWithCurrentWeapon();
#pragma endregion Functions
};
