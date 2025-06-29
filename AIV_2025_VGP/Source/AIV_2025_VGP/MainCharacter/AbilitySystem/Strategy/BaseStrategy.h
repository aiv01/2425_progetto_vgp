// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti
// Andreea Manuela Hodor
// Marco Pungillo

#pragma once

#include "CoreMinimal.h"
#include "../../Interfaces/I_FunctionStrategy.h"
#include "GameFramework/Character.h"
#include "BaseStrategy.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew)
class AIV_2025_VGP_API UBaseStrategy : public UObject, public II_FunctionStrategy
{
	GENERATED_BODY()

public:
	virtual void Execute(AActor* Executor) override;
	virtual void Execute() override;
	
	UPROPERTY(BlueprintReadWrite, Category = "Params")
	ACharacter* StrategyOwner;
};
