// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele
#pragma once

#include "CoreMinimal.h"
#include "GridProcessorComponent.h"
#include "GridPreviewComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class AIV_2025_VGP_API UGridPreviewComponent : public UGridProcessorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Preview/Material")
	UMaterial* PreviewMaterial;
	//TObjectPtr<UMaterial> PreviewMaterial;
	
	void ShowPreview(const FHitResult HitResult, FGridSurface*& CloserSurface);
	
};
