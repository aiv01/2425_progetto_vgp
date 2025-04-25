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
	AStaticMeshActor* PreviewMesh;
	
	void ShowPreview(const FHitResult& HitResult, UMaterial* PositiveMeshMaterial, UMaterial* NegativeMeshMaterial, FGridSurface*& CloserSurface, const FName TrapRowName);
	void ClearPreviewMesh();
};
