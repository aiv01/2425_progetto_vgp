// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "WaveSpawner.generated.h"

UCLASS(BlueprintType)
class WAVETOOL_API AWaveSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AWaveSpawner();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	UBoxComponent* SpawnBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnWeight = 1;
	
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	FVector GetRandomSpawnLocation() const;
};
