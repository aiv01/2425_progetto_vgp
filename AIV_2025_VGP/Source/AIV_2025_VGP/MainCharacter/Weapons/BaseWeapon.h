// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS(Blueprintable,BlueprintType)
class AIV_2025_VGP_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();

#pragma region Properties
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Weapon Params")
	FString WeaponName;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Weapon Params")
	FString WeaponDamage;
	
#pragma endregion Properties

#pragma region Functions
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category="Weapon")
	void AttackEvent();
#pragma endregion Functions
};
