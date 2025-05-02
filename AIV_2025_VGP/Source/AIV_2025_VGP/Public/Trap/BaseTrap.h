// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseTrap.generated.h"


UCLASS()
class AIV_2025_VGP_API ABaseTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseTrap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Properties")
	FVector TrapSlotSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Property")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Property")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrapProperty")
	int32 TrapCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrapProperty")
	FTimerHandle ResetTimerHandle;

	// Cooldown dopo l'attivazione
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Properties")
	float CooldownTime = 2.0f;

	bool bCanActivate = true;

	// Funzione richiamata dopo il timer
	void EnableTrap();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Trap")
	virtual void ActivateTrap();

	UFUNCTION(BlueprintCallable, Category = "Trap")
	virtual void ResetTrap();
};
