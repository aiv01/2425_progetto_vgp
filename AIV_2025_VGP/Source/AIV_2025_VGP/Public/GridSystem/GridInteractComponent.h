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
	TArray<AGridGeneratorVolume*> GridVolumesRef;
	AGridGeneratorVolume* ActualVolumeRef;
	 
	
	UPROPERTY(EditInstanceOnly, Category="GridGenerator")
	float InteractDistance;

private:
	AGridGeneratorVolume* GetCloserVolume(FVector Position);

public:	
	// Called every frame
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void SetVolumeRef(AGridGeneratorVolume* OtherVolumeRef);
	UFUNCTION(BlueprintPure)
	void GridRayCast(FVector CameraForward, FHitResult& result, bool& Hit, UPARAM(ref) FGridSurface& CloserGridSurface);
	
	UFUNCTION(BlueprintCallable)
	bool IsPositionWithinActualVolume(const FVector Position);
	UFUNCTION(BlueprintCallable)
	bool IsPositionWithinVolume(AGridGeneratorVolume* VolumeRef, const FVector Position);
	
	
	UFUNCTION(BlueprintCallable)
	void ShowPreview(UPARAM(ref) FGridSurface& GridSurface);
	void PlaceTrap(UPARAM(ref)FGridSurface& GridSurface);
		
};
