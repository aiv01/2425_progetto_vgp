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
	AGridGeneratorVolume* LastGridVolume;
	int TrapIndexer = 0;
	FName CurrentTrap;

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> AvailableTraps;

private:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, Category="GridGenerator|Preview")
	//TObjectPtr<UMaterial> PreviewMaterial;
	UMaterial* PositivePreviewMaterial;

	UPROPERTY(EditAnywhere, Category="GridGenerator|Preview")
	//TObjectPtr<UMaterial> PreviewMaterial;
	UMaterial* NegativePreviewMaterial;
	
	UPROPERTY(EditAnywhere, Category="GridGenerator|Collision")
	float InteractDistance = 600.0;
	
	UPROPERTY(EditAnywhere, Category="GridGenerator|Collision")
	float SphereCastRadius = 5.0;
	
	UPROPERTY(EditAnywhere, Category="GridGenerator|Collision")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;

	UPROPERTY(EditAnywhere, Category="GridGenerator|Debug")
	bool bDebug = false;
	
	UPROPERTY(EditAnywhere, Category="GridGenerator|Debug")
	FColor DebugColor;

protected:
	UFUNCTION(BlueprintPure)
	void GridRayCast(FVector CameraForward, FHitResult& result, bool& Hit);

public:

	UFUNCTION(BlueprintCallable)
	bool IsPositionWithinVolume(AGridGeneratorVolume* VolumeRef, const FVector Position);

	UFUNCTION(BlueprintCallable)
	void ShowPreview(FVector CameraForward, bool& HitSurface, const FName TrapRowName);
	
	UFUNCTION(BlueprintCallable)
	void PlaceTrap(FVector CameraForward, bool& HitSurface, const FName TrapRowName);

	UFUNCTION(BlueprintCallable)
	FName GetCurrentTrapName() const;

	UFUNCTION(BlueprintCallable)
	void SwapTrap(int Sign);

	UFUNCTION(BlueprintCallable)
	void StopPreview();
};
