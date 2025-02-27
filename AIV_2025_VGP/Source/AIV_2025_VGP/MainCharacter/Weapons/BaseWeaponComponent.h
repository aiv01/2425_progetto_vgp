// FABIO GIANNINO
// LUCA CASAMENTI

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

public:
	UPROPERTY(EditAnywhere, Category="Weapon")
	TArray<TSubclassOf<ABaseWeapon>> WeaponClasses;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* CurrentWeapon;

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(int32 indexToEquip);

	UFUNCTION(BlueprintCallable)
	void ChangeWeapon(bool bForward);

	UFUNCTION(BlueprintCallable)
	void SpawnWeapons(USkeletalMeshComponent* PlayerMesh);

private:
	UPROPERTY(VisibleAnywhere)
	TArray<ABaseWeapon*> Weapons;
	UPROPERTY(VisibleAnywhere)
	int32 CurrentWeaponIndex = 0;
};
