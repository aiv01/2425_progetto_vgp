// Fill out your copyright notice in the Description page of Project Settings.
// Marco Pungillo

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GOAPAgentComponent.h"
#include "GOAPAction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew)
class AIV_2025_VGP_API UGOAPAction : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, float> Satisfiers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxInsistence = 10.0f;
	UPROPERTY(BlueprintReadWrite)
	APawn* Executer;
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	UGOAPWorldModel* ElaborateWorldModel(UGOAPWorldModel* Wm);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ExecAction();

	UFUNCTION(BlueprintCallable)
	float GetGoalChange(const FGOAPGoal goal) const;
};
