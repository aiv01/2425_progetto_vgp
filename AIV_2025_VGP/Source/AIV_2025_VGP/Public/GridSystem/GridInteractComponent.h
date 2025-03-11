// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridGeneratorVolume.h"
#include "Components/ActorComponent.h"
#include "GridInteractComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class AIV_2025_VGP_API UGridInteractComponent : public UActorComponent
{
	GENERATED_BODY()


	
protected:
	UPROPERTY(EditAnywhere, Category="GridGenerator|Collision")
	float InteractDistance = 300.0;
	
	UPROPERTY(EditAnywhere, Category="GridGenerator|Collision")
	float SphereCastRadius = 100.0;
	
	UPROPERTY(EditAnywhere, Category="GridGenerator|Collision")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;

	UPROPERTY(EditAnywhere, Category="GridGenerator|Debug")
	bool bDebug = false;
	UPROPERTY(EditAnywhere, Category="GridGenerator|Debug")
	FColor DebugColor;
	
public:
	UFUNCTION(BlueprintPure)
	void GridRayCast(FVector CameraForward, FHitResult& result, bool& Hit, AGridGeneratorVolume*& VolumeRef);

	UFUNCTION(BlueprintCallable)
	bool IsPositionWithinVolume(AGridGeneratorVolume* VolumeRef, const FVector Position);

	UFUNCTION(BlueprintCallable)
	void ShowPreview(FVector CameraForward, bool& HitSurface);
	
	UFUNCTION(BlueprintCallable)
	void PlaceTrap(FVector CameraForward, bool& HitSurface);
		
};
