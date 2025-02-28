// FABIO GIANNINO

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponHit);

UCLASS(Blueprintable,BlueprintType)
class AIV_2025_VGP_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, Category="Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(BlueprintAssignable,BlueprintCallable, Category = "Weapon")
	FOnWeaponHit OnWeaponHit;
};
