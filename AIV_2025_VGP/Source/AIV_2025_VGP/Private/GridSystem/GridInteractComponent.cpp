#include "GridSystem/GridInteractComponent.h"
#include "GridSystem/GridPlacementComponent.h"
#include "GridSystem/GridPreviewComponent.h"
#include "Kismet/GameplayStatics.h"


void UGridInteractComponent::GridRayCast (FVector CameraForward, FHitResult& result, bool& Hit, AGridGeneratorVolume*& VolumeRef)
{
	//do a line trace between the player character and it's camera forward
	const FVector ownerLocation = GetOwner()->GetActorLocation();
	const FVector endLocation = ownerLocation + CameraForward * InteractDistance;
	//Check if collide to wall or floor
	if(bDebug)
	{
		DrawDebugLine(GetWorld(), ownerLocation, endLocation, DebugColor, false, 10, 0, 5);
	}
	if(GetWorld()->LineTraceSingleByChannel(result, ownerLocation, endLocation, ECollisionChannel::ECC_Visibility))
	{
		if(bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("HIT WITH SURFACE"));
			DrawDebugSphere(GetWorld(), result.Location, SphereCastRadius, 12, DebugColor, false, 10, 0, 5);
		}
		
		//check the volume inside the SphereOverlapp 
		TArray<AActor*> OverlappedActors;
		TArray<AActor*> ignoreActor = { GetOwner() };
		if(UKismetSystemLibrary::SphereOverlapActors(GetWorld(), result.Location, SphereCastRadius, ObjectTypeQuery, AGridGeneratorVolume::StaticClass(), ignoreActor, OverlappedActors))
		{
			if(bDebug)
			{
				UE_LOG(LogTemp, Warning, TEXT("HIT WITH VOLUME"));
				for (auto OverlappedActor : OverlappedActors)
				{
					UE_LOG(LogTemp, Warning, TEXT("ACTOR OVERLAPPED: %s"), *OverlappedActor->GetName());
				}
			}
			//get the first overlapped volume
			if(!OverlappedActors.IsEmpty())
			{
				VolumeRef = Cast<AGridGeneratorVolume>(OverlappedActors[0]);
				Hit = true;
				return;
			}
		}
	}
	Hit = false;
}




/**
 * Given a World Position and a volume, check if it's Inside the Box Volume Bounds (the volume is null, return false)
 * @param VolumeRef Volume to check
 * @param Position World Position to Check
 * @return bool
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
 * Given a Grid Surface, Show the Trap mesh preview. 
 * @param GridSurface 
 */
void UGridInteractComponent::ShowPreview(FVector CameraForward, bool& HitSurface)
{
	FHitResult result;
	AGridGeneratorVolume* GridVolumeRef;
	GridRayCast(CameraForward, result, HitSurface, GridVolumeRef);
	if(HitSurface && GridVolumeRef)
	{
		UGridPreviewComponent* UGridPreviewComp = GridVolumeRef->FindComponentByClass<UGridPreviewComponent>();
		if(UGridPreviewComp)
		{
			//UGridPreviewComp->ShowPreview()
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
 * Given a Grid Surface, Place the Trap and occupy the grid surface.
 * @param GridSurface 
 */
void UGridInteractComponent::PlaceTrap (FVector CameraForward, bool& HitSurface)
{
	FHitResult result;
	AGridGeneratorVolume* GridVolumeRef;
	GridRayCast(CameraForward, result, HitSurface, GridVolumeRef);
	if(HitSurface && GridVolumeRef)
	{
		UGridPlacementComponent* UGridPlaceComp = GridVolumeRef->FindComponentByClass<UGridPlacementComponent>();
		if(UGridPlaceComp)
		{
			//place trap
			//UGridPlaceComp->PlaceTrap();
		}else if(bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("NO PLACE COMPONENT ATTACHED"));
		}
	}else if(bDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO VOLUME FOUND"));
	}
}
