// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridPlacementComponent.h"
#include "GridPreviewComponent.h"
#include "UObject/Interface.h"
#include "ProcessorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProcessorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AIV_2025_VGP_API IProcessorInterface
{
	GENERATED_BODY()
	
public:
	// = 0 mean that is "Pure Virtual"
	virtual void BindPreviewInteract(UGridPreviewComponent* GridPreviewComponent) = 0;
	virtual void BindPlacementInteract(UGridPlacementComponent* GridPlacementComponent) = 0;
};
