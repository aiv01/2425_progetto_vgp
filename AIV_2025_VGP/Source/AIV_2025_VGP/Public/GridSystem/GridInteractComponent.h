// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele
#pragma once

#include "CoreMinimal.h"
#include "GridGeneratorVolume.h"
#include "Components/ActorComponent.h"
#include "GridInteractComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class AIV_2025_VGP_API UGridInteractComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	FGridSurface* LastGridSurface;
	
public:
	UPROPERTY(EditAnywhere, Category="GridGenerator|Collision")
	float InteractDistance = 600.0;
	
	UPROPERTY(EditAnywhere, Category="GridGenerator|Collision")
	float SphereCastRadius = 100.0;
	
	UPROPERTY(EditAnywhere, Category="GridGenerator|Collision")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;

	UPROPERTY(EditAnywhere, Category="GridGenerator|Debug")
	bool bDebug = false;
	
	UPROPERTY(EditAnywhere, Category="GridGenerator|Debug")
	FColor DebugColor;

protected:
	UFUNCTION(BlueprintPure)
	void GridRayCast(FVector CameraForward, FHitResult& result, bool& Hit, AGridGeneratorVolume*& VolumeRef);

public:

	UFUNCTION(BlueprintCallable)
	bool IsPositionWithinVolume(AGridGeneratorVolume* VolumeRef, const FVector Position);

	UFUNCTION(BlueprintCallable)
	void ShowPreview(FVector CameraForward, bool& HitSurface);
	
	UFUNCTION(BlueprintCallable)
	void PlaceTrap(FVector CameraForward, bool& HitSurface);
		
};
