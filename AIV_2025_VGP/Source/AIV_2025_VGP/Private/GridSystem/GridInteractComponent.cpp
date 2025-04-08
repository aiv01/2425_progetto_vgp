// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele

#include "GridSystem/GridInteractComponent.h"

#include "Engine/StaticMeshActor.h"
#include "GridSystem/GridPlacementComponent.h"
#include "GridSystem/GridPreviewComponent.h"
#include "Kismet/GameplayStatics.h"

/**
 * this method does a line trace to a wall or floor. if there is a collision, it checks if there is an AGridGeneratorVolume and returns it (if there are more, it returns one)
 * @param CameraForward FVector (look direction)
 * @param result FHitResult&
 * @param Hit bool
 */
void UGridInteractComponent::GridRayCast (FVector CameraForward, FHitResult& result, bool& Hit)
{
	//do a line trace between the player character and it's camera forward
	const FVector ownerLocation = GetOwner()->GetActorLocation();
	const FVector endLocation = ownerLocation + CameraForward * InteractDistance;
	//draw the debug line for the line trace
	if(bDebug)
	{
		DrawDebugLine(GetWorld(), ownerLocation, endLocation, DebugColor, false, -1, 0, 5);
	}

	//Perform the actual line trace
	if(GetWorld()->LineTraceSingleByChannel(result, ownerLocation, endLocation, ECC_Visibility))
	{
		//draw the debug Sphere for the SphereOverlap
		if(bDebug)
		{
			UE_LOG(LogTemp, Log, TEXT("HIT WITH SURFACE"));
			DrawDebugSphere(GetWorld(), result.Location, SphereCastRadius, 12, DebugColor, false, -1, 0, 5);
		}
		
		// Check for overlapping GridVolume actors in a spherical area around the hit
		TArray<AActor*> OverlappedActors;
		TArray<AActor*> ignoreActor = { GetOwner() };
		
		if(UKismetSystemLibrary::SphereOverlapActors(GetWorld(), result.Location, SphereCastRadius, ObjectTypeQuery, AGridGeneratorVolume::StaticClass(), ignoreActor, OverlappedActors))
		{
			//debug for the hit result and the actors inside sphere
			if(bDebug)
			{
				UE_LOG(LogTemp, Log, TEXT("HIT WITH VOLUME"));
				for (auto OverlappedActor : OverlappedActors)
				{
					UE_LOG(LogTemp, Log, TEXT("ACTOR OVERLAPPED: %s"), *OverlappedActor->GetName());
				}
			}
			
			// Set the first overlapped volume as the current grid volume
			if(!OverlappedActors.IsEmpty())
			{
				LastGridVolume = Cast<AGridGeneratorVolume>(OverlappedActors[0]);
				Hit = true;
				return;
			}
		} else if (LastGridVolume) // If no volume is found but one was previously selected
		{
			if (bDebug)
			{
				UE_LOG(LogTemp, Warning, TEXT("NO GridGeneratorVolume FOUND"));
			}
			// Clear the preview mesh on the last volume
			UGridPreviewComponent* UGridPreviewComp = LastGridVolume->FindComponentByClass<UGridPreviewComponent>();
			if(UGridPreviewComp)
			{
				UGridPreviewComp->ClearPreviewMesh();
			}

			// Reset references
			LastGridVolume = nullptr;
			LastGridSurface = nullptr;
		} 
	} else if(bDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO SURFACE FOUND"));
	}
	Hit = false;
}

/**
 * DEPRECATED
 * Given a World Position and a volume, check if it's Inside the Box Volume Bounds (the volume is null, return false)
 * @param VolumeRef Volume to check
 * @param Position World Position to Check
 * @return bool
 * 
 */
bool UGridInteractComponent::IsPositionWithinVolume (AGridGeneratorVolume* VolumeRef, FVector Position)
{
	if(!VolumeRef) return false;
	FVector BoxExtent = VolumeRef->GetBounds().GetBox().GetExtent();

	// Check if Position is within bounds of the VolumeRef
	return ((VolumeRef->GetActorLocation().X - BoxExtent.X) > Position.X && (VolumeRef->GetActorLocation().X + BoxExtent.X) < Position.X &&
		(VolumeRef->GetActorLocation().Y - BoxExtent.Y) > Position.Y && (VolumeRef->GetActorLocation().Y + BoxExtent.Y) < Position.Y &&
		(VolumeRef->GetActorLocation().Z - BoxExtent.Z) > Position.Z && (VolumeRef->GetActorLocation().Z + BoxExtent.Z) < Position.Z);
}

/**
 * Given the CameraForward, Show the Trap mesh preview in a specific point. 
 * @param CameraForward FVector (look direction)
 * @param HitSurface bool
 */
void UGridInteractComponent::ShowPreview(FVector CameraForward, bool& HitSurface, const FName TrapRowName)
{
	//call the GridRayCast and return the Volume and HitResult
	FHitResult Result;
	GridRayCast(CameraForward, Result, HitSurface);
	//if HitSurface and GridVolumeRef are valid
	if(HitSurface && LastGridVolume)
	{
		//if the dirty flag is valid
		if(LastGridSurface)
		{
			//check if the hit location is still inside the cell bounds
			if(LastGridVolume->IsPointInsideGridSurface(*LastGridSurface, Result.Location))
			{
				// Optionally draw debug for confirmation
				if(bDebug)
				{
					UE_LOG(LogTemp, Log, TEXT("Still Inside"));
					UGridPreviewComponent* UGridPreviewCompDebug = LastGridVolume->FindComponentByClass<UGridPreviewComponent>();
					UGridPreviewCompDebug->DrawDebug(LastGridSurface);
				}
				return; // No need to update preview
			}
			// We've left the previous surface
			if(bDebug)
			{
				UE_LOG(LogTemp, Log, TEXT("no more inside"));
			}
		}
		// Get the preview component and show the new preview
		UGridPreviewComponent* UGridPreviewComp = LastGridVolume->FindComponentByClass<UGridPreviewComponent>();
		if(UGridPreviewComp)
		{
			//if there is the component, call the preview method inside
			UGridPreviewComp->ShowPreview(Result, PositivePreviewMaterial, NegativePreviewMaterial, LastGridSurface, TrapRowName);
			if(bDebug){
				UE_LOG(LogTemp, Log, TEXT("PREVIEW COMPONENT ATTACHED :)"));
			}

		}else if(bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("NO PREVIEW COMPONENT ATTACHED"));
		}
	}else if(bDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO VOLUME FOUND"));
	}
}

/**
 * Given the CameraForward, Place the Trap in a specific point Inside the volume. 
 * @param CameraForward FVector (look direction)
 * @param HitSurface bool
 */ 
void UGridInteractComponent::PlaceTrap (FVector CameraForward, bool& HitSurface, const FName TrapRowName)
{
	//call the GridRayCast and return the Volume and HitResult
	FHitResult Result;
	GridRayCast(CameraForward, Result, HitSurface);
	//if HitSurface and GridVolumeRef are valid
	if(HitSurface && LastGridVolume)
	{
		//from the hit location call the volume component 
		UGridPlacementComponent* UGridPlaceComp = LastGridVolume->FindComponentByClass<UGridPlacementComponent>();
		if(UGridPlaceComp)
		{
			//if there is the component, call the Place method inside
			if(UGridPlaceComp->PlaceTrap(Result, TrapRowName))
			{
				LastGridSurface = nullptr;
			}
			if(bDebug){
				UE_LOG(LogTemp, Log, TEXT("PLACE COMPONENT ATTACHED :)"));
			}

		}else if(bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("NO PLACE COMPONENT ATTACHED"));
		}
	}else if(bDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO VOLUME FOUND"));
	}
}
