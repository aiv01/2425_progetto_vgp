// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GOAPWorldModel.generated.h"

struct FGOAPGoal;
USTRUCT(Blueprintable)
struct FWorldModel
{
	float MaxParameterInsistence = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGOAPGoal> DistanceModel;


	GENERATED_BODY()
};


class UGOAPAction;
UCLASS(Blueprintable)
class AIV_2025_VGP_API UGOAPWorldModel : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWorldModel Model;

	int32 CurrentActionIndex = 0;
	UPROPERTY(Instanced)
	TArray<UGOAPAction*> UsableActions;

	UFUNCTION(BlueprintCallable)
	virtual float CalculateDiscontentment();
	UFUNCTION(BlueprintCallable)
	virtual UGOAPAction* NextAction();
	UFUNCTION(BlueprintCallable)
	virtual void ApplyAction(const UGOAPAction* Action);
};
