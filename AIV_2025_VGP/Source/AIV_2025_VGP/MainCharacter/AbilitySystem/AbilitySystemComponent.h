// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Strategy/BaseStrategy.h"
#include "AbilitySystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIV_2025_VGP_API UAbilitySystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilitySystemComponent();

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Strategy", meta = (ExposeOnSpawn = "true"))
	TMap<FName, UBaseStrategy*> StrategyInstances;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Strategy")
	void ExecuteStrategy(FName StrategyName);

	UFUNCTION(BlueprintCallable, Category = "Strategy")
	UBaseStrategy* ClassTypeGetExecutedStrategy(FName StrategyName);

	UFUNCTION(BlueprintCallable, Category = "Strategy")
	bool GetStrategyParamGeneric(FName StrategyName, const FString& ParamName, FString& OutValue);
	
};
