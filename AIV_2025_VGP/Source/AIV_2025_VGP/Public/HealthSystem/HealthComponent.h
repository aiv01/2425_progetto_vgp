// Copyright Epic Games, Inc. All Rights Reserved.
// Lorenzo Petrillo

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIV_2025_VGP_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	
	// Max Health 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	//Current Health r only
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	// Sets health to a current value
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHealth(float Amount);

	// Adds health to current health
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	// Removes health to current health
	UFUNCTION(BlueprintCallable, Category = "Health")
	void RemoveHealth(float Amount);





	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
