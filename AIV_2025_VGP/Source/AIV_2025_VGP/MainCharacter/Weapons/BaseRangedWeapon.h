// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "BaseRangedWeapon.generated.h"

UCLASS()
class AIV_2025_VGP_API ABaseRangedWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	ABaseRangedWeapon();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Ranged Weapon Params")
	int32 MaxBullets;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Ranged Weapon Params")
	int32 CurrentBullets;

protected:
	virtual void BeginPlay() override;	
	
public:
	UFUNCTION(BlueprintCallable)
	int32 GetMaxBullets() const;
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentBullets() const;
	UFUNCTION(BlueprintCallable)
	int32 Reload(int32 AmmoAvailable);
	
	virtual bool CanAttack() const override;
};
