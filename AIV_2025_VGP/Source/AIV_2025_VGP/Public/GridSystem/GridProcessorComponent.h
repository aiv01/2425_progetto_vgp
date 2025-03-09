// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FGridSurface.h"
#include "Components/ActorComponent.h"
#include "GridProcessorComponent.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIV_2025_VGP_API UGridProcessorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void InitializeComponent() override;
	
protected:
	//reference of the GridData from the GridGeneratorVolumeOwner Owner
	TArray<FGridSurface> *GridData;
	
};
