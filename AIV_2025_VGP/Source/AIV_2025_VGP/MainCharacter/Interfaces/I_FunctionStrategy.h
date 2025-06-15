// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_FunctionStrategy.generated.h"

UINTERFACE(Blueprintable)
class AIV_2025_VGP_API UI_FunctionStrategy: public UInterface
{
	GENERATED_BODY()
};

class AIV_2025_VGP_API II_FunctionStrategy
{
	GENERATED_BODY()
public:
	virtual void Execute(AActor* Executor) = 0;
	virtual void Execute() = 0;
};