// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../Interfaces/I_FunctionStrategy.h"
#include "BaseStrategy.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew)
class AIV_2025_VGP_API UBaseStrategy : public UObject, public II_FunctionStrategy
{
	GENERATED_BODY()

public:
	virtual void Execute() override;
	
};
