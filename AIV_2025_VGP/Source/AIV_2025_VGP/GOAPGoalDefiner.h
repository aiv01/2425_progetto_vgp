// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GOAPGoalDefiner.generated.h"

/**
 * 
 */
class ABaseEnemy;;
class UGOAPWorldModel;
UCLASS(Blueprintable, EditInlineNew)
class AIV_2025_VGP_API UGOAPGoalDefiner : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
	ABaseEnemy* Executer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Insistence")
	float MinInsistence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Insistence")
	float MaxInsistence = 10.0f;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	UGOAPWorldModel* ElaborateWorldModel(UGOAPWorldModel* Wm);
	
};
