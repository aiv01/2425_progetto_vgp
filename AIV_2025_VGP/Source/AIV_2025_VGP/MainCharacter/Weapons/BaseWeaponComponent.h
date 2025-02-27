// FABIOGIANNINO

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
	TSubclassOf<ABaseWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category="Weapon")
	TSubclassOf<ABaseWeapon> SecondaryWeaponClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* CurrentWeapon;

	void EquipWeapon(ABaseWeapon* NewWeapon, USkeletalMeshComponent* PlayerMesh);

	UFUNCTION(BlueprintCallable)
	void ChangeWeapon();

	
	
};
