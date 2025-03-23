// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoolComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIV_2025_VGP_API UPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPoolComponent();

	UFUNCTION(BlueprintCallable, Category="Pooler")
	AActor* GetPoolElement(FVector location, FRotator rotation);
	UFUNCTION(BlueprintCallable, Category="Pooler")
	void InitializePool();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	
public:	
	UPROPERTY(EditAnywhere, Category="Pooler")
	TSubclassOf<AActor> PoolElementClass;

	UPROPERTY(EditAnywhere, Category="Pooler")
	int32 PoolSize = 15;
	
	UPROPERTY(BlueprintReadOnly, Category="Pooler")
	TArray<AActor*> PoolList;	
};
