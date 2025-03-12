// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#pragma once

#include "CoreMinimal.h"
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
