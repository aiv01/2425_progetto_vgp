// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GOAPWorldModel.generated.h"

class UGOAPAction;
UCLASS(Blueprintable)
class AIV_2025_VGP_API UGOAPWorldModel : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	virtual float calculateDiscontentment();
	UFUNCTION(BlueprintCallable)
	virtual UGOAPAction* NextAction();
	UFUNCTION(BlueprintCallable)
	virtual void ApplyAction(const UGOAPAction* action);
};
