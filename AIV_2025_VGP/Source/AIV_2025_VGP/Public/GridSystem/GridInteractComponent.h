// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProcessorInterface.h"
#include "Components/ActorComponent.h"
#include "GridInteractComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIV_2025_VGP_API UGridInteractComponent : public UActorComponent, public IProcessorInterface
{
	GENERATED_BODY()
	DECLARE_DELEGATE_OneParam(FShowPreview, FVector);
	DECLARE_DELEGATE_OneParam(FPlaceTrap, FGridSurface*);



public:	
	// Called every frame
	FPlaceTrap OnPlaceTrapTriggered;
	FShowPreview OnShowPreviewTriggered;
	virtual void BindPreviewInteract(UGridPreviewComponent* GridPreviewComponent) override;
	virtual void BindPlacementInteract(UGridPlacementComponent* GridPlacementComponent) override;

	UFUNCTION(Blueprintable)
	void ShowPreview(const FVector Point);
	void PlaceTrap(FGridSurface* GridSurface);
		
};
