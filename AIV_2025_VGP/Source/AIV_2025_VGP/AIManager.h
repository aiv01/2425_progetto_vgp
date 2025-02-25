// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPWorldModel.h"
#include "GameFramework/Actor.h"
#include "AIManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWorldStateUpdate, const UGOAPWorldModel*, WorldState);
UCLASS(Blueprintable)
class AIV_2025_VGP_API AAIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIManager();

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="WorldState")
	FWorldStateUpdate NotifyWorldUpdate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
