// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele

#include "GridSystem/GridInteractComponent.h"
#include "GridSystem/GridPlacementComponent.h"
#include "GridSystem/GridPreviewComponent.h"
#include "Kismet/GameplayStatics.h"

/**
 * this method does a line trace to a wall or floor. if there is a collision, it checks if there is an AGridGeneratorVolume and returns it (if there are more, it returns one)
 * @param CameraForward FVector (look direction)
 * @param result FHitResult&
 * @param Hit bool
 * @param VolumeRef AGridGeneratorVolume*&
 */
void UGridInteractComponent::GridRayCast (FVector CameraForward, FHitResult& result, bool& Hit, AGridGeneratorVolume*& VolumeRef)
{
	//do a line trace between the player character and it's camera forward
	const FVector ownerLocation = GetOwner()->GetActorLocation();
	const FVector endLocation = ownerLocation + CameraForward * InteractDistance;
	//Check if collide to wall or floor
	if(bDebug)
	{
		DrawDebugLine(GetWorld(), ownerLocation, endLocation, DebugColor, false, -1, 0, 5);
	}
	if(GetWorld()->LineTraceSingleByChannel(result, ownerLocation, endLocation, ECC_Visibility))
	{
		if(bDebug)
		{
			UE_LOG(LogTemp, Log, TEXT("HIT WITH SURFACE"));
			DrawDebugSphere(GetWorld(), result.Location, SphereCastRadius, 12, DebugColor, false, -1, 0, 5);
		}
		//if the line trace hit a surface (wall or floor)
		//check if there is a volume inside a SphereOverlapActors 
		TArray<AActor*> OverlappedActors;
		TArray<AActor*> ignoreActor = { GetOwner() };
		if(UKismetSystemLibrary::SphereOverlapActors(GetWorld(), result.Location, SphereCastRadius, ObjectTypeQuery, AGridGeneratorVolume::StaticClass(), ignoreActor, OverlappedActors))
		{
			if(bDebug)
			{
				UE_LOG(LogTemp, Log, TEXT("HIT WITH VOLUME"));
				for (auto OverlappedActor : OverlappedActors)
				{
					UE_LOG(LogTemp, Log, TEXT("ACTOR OVERLAPPED: %s"), *OverlappedActor->GetName());
				}
			}
			//get the first overlapped volume
			if(!OverlappedActors.IsEmpty())
			{
				VolumeRef = Cast<AGridGeneratorVolume>(OverlappedActors[0]);
				Hit = true;
				return;
			}
		} else if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("NO GridGeneratorVolume FOUND"));
		}
	} else if(bDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO SURFACE FOUND"));
	}
	Hit = false;
}

/**
 * Given a World Position and a volume, check if it's Inside the Box Volume Bounds (the volume is null, return false)
 * @param VolumeRef Volume to check
 * @param Position World Position to Check
 * @return bool
 * DEPRECATED
 */
bool UGridInteractComponent::IsPositionWithinVolume (AGridGeneratorVolume* VolumeRef, FVector Position)
{
	if(!VolumeRef) return false;
	FVector BoxExtent = VolumeRef->GetBounds().GetBox().GetExtent();
	return ((VolumeRef->GetActorLocation().X - BoxExtent.X) > Position.X && (VolumeRef->GetActorLocation().X + BoxExtent.X) < Position.X &&
		(VolumeRef->GetActorLocation().Y - BoxExtent.Y) > Position.Y && (VolumeRef->GetActorLocation().Y + BoxExtent.Y) < Position.Y &&
		(VolumeRef->GetActorLocation().Z - BoxExtent.Z) > Position.Z && (VolumeRef->GetActorLocation().Z + BoxExtent.Z) < Position.Z);
}

/**
 * Given the CameraForward, Show the Trap mesh preview in a specific point. 
 * @param CameraForward FVector (look direction)
 * @param HitSurface bool
 */
void UGridInteractComponent::ShowPreview(FVector CameraForward, bool& HitSurface)
{
	//call the GridRayCast and return the Volume and HitResult
	FHitResult Result;
	AGridGeneratorVolume* GridVolumeRef;
	GridRayCast(CameraForward, Result, HitSurface, GridVolumeRef);
	//if HitSurface and GridVolumeRef are valid
	if(HitSurface && GridVolumeRef)
	{
		//if the dirty flag is valid
		if(LastGridSurface)
		{
			//check if the hit location is still inside the cell bounds
			if(GridVolumeRef->IsPointInsideGridSurface(*LastGridSurface, Result.Location))
			{
				if(bDebug)
				{
					UE_LOG(LogTemp, Log, TEXT("Still Inside"));
				}
				return;
			}
			if(bDebug)
			{
				UE_LOG(LogTemp, Log, TEXT("no more inside"));
			}
		}
		//if LastGridSurface is null or different from the hit location call the volume preview component 
		UGridPreviewComponent* UGridPreviewComp = GridVolumeRef->FindComponentByClass<UGridPreviewComponent>();
		if(UGridPreviewComp)
		{
			//if there is the component, call the preview method inside
			UGridPreviewComp->ShowPreview(Result, LastGridSurface);
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
void UGridInteractComponent::PlaceTrap (FVector CameraForward, bool& HitSurface)
{
	//call the GridRayCast and return the Volume and HitResult
	FHitResult Result;
	AGridGeneratorVolume* GridVolumeRef;
	GridRayCast(CameraForward, Result, HitSurface, GridVolumeRef);
	//if HitSurface and GridVolumeRef are valid
	if(HitSurface && GridVolumeRef)
	{
		//from the hit location call the volume component 
		UGridPlacementComponent* UGridPlaceComp = GridVolumeRef->FindComponentByClass<UGridPlacementComponent>();
		if(UGridPlaceComp)
		{
			//if there is the component, call the Place method inside
			UGridPlaceComp->PlaceTrap(Result);
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
