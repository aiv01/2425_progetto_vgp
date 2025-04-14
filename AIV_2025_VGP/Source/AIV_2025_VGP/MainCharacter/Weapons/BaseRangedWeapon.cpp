// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseRangedWeapon.h"


ABaseRangedWeapon::ABaseRangedWeapon()
{
	bCanAttack= true;
}


void ABaseRangedWeapon::BeginPlay()
{
	Super::BeginPlay();	
}

bool ABaseRangedWeapon::CanAttack() const
{
	return CurrentBullets > 0 && bCanAttack;
}

int32 ABaseRangedWeapon::GetMaxBullets() const
{
	return MaxBullets;
}

int32 ABaseRangedWeapon::GetCurrentBullets() const
{
	return CurrentBullets;
}

int32 ABaseRangedWeapon::Reload(int32 AmmoAvailable)
{
	int32 MissingAmmo = MaxBullets - CurrentBullets;

	if (MissingAmmo <= 0 || AmmoAvailable <= 0)
	{
		return 0;
	}

	int32 AmmoToReload = FMath::Min(MissingAmmo, AmmoAvailable);
	CurrentBullets += AmmoToReload;

	return AmmoToReload; 
}
