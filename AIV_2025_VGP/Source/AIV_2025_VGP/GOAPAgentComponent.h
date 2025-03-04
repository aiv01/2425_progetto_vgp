// Fill out your copyright notice in the Description page of Project Settings.
// Marco Pungillo
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GOAPAgentComponent.generated.h"

USTRUCT(BlueprintType)
struct FGOAPGoal
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;

	float GetDiscontentment(float newValue)
	{
		newValue = FMath::CeilToInt(newValue);
		return newValue * newValue;
	}
};

class UGOAPAction;
class UGOAPWorldModel;
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIV_2025_VGP_API UGOAPAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGOAPAgentComponent();

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "GOAP Actions")
	TArray<UGOAPAction*> Actions;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual UGOAPAction* PlanAction(UGOAPWorldModel* wdModel, const int maxDepth);
};
