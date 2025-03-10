#include "GridSystem/GridInteractComponent.h"

#include "GridSystem/FGridSurface.h"
#include "GridSystem/GridPlacementComponent.h"
#include "GridSystem/GridPreviewComponent.h"
#include "Kismet/GameplayStatics.h"

void UGridInteractComponent::BeginPlay ()
{
	//Parent BeginPlay
	Super::BeginPlay();

	//get all grid volume actor in the map
	TArray<AActor*> AActorOfClass;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridGeneratorVolume::StaticClass(), AActorOfClass);
	//TEMP, need to fine alternative for cast
	for(auto actorRef : AActorOfClass)
	{
		GridVolumesRef.Add(Cast<AGridGeneratorVolume>(actorRef));
	}

	//set the volume ref for the first time
	//GetCloserVolume(GetOwner()->GetActorLocation());
}

//set the actual volume ref
void UGridInteractComponent::SetVolumeRef (AGridGeneratorVolume* OtherVolumeRef)
{
	if(ActualVolumeRef == OtherVolumeRef) return;
	ActualVolumeRef = OtherVolumeRef;
}

void UGridInteractComponent::GridRayCast (FVector CameraForward, FHitResult& result, bool& Hit, FGridSurface& CloserGridSurface)
{
	//do a line trace between the player character and it's camera forward
	const FVector ownerLocation = GetOwner()->GetActorLocation();
	const FVector endLocation = ownerLocation + CameraForward * InteractDistance;
	if(GetWorld()->LineTraceSingleByChannel(result, ownerLocation, endLocation, ECollisionChannel::ECC_Visibility))
	{
		if(!IsPositionWithinActualVolume(result.Location))
		{
			SetVolumeRef(GetCloserVolume(result.Location));
		}
		if(!ActualVolumeRef)
		{
			Hit = false;
			return;
		}
		
		ActualVolumeRef->GetCloserSurface(result, CloserGridSurface);
		Hit = true;
		return;
	}
	Hit = false;
}

/**
 * Given a World Position, check if it's inside any Grid Volume in the map
 * @param Position 
 * @return if inside a volume the AGridGeneratorVolume, else NULL
 */
AGridGeneratorVolume* UGridInteractComponent::GetCloserVolume (FVector Position)
{
	for (auto GridVolume : GridVolumesRef)
	{
		if(IsPositionWithinActualVolume(Position))
		{
			return GridVolume;
		}
	}
	return NULL;
}

/**
 * Given a World Position, check if it's Inside Referenced Box Volume Bounds (the volume is null, return false)
 * @param Position World Position to Check
 * @return bool
 */
bool UGridInteractComponent::IsPositionWithinActualVolume (FVector Position)
{
	if(!ActualVolumeRef) return false;
	return IsPositionWithinVolume(ActualVolumeRef, Position);
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
void UGridInteractComponent::ShowPreview (FGridSurface& GridSurface)
{
	if(!GridSurface.bOccupied)
	{
		UGridPreviewComponent* UGridPreviewComp = ActualVolumeRef->FindComponentByClass<UGridPreviewComponent>();
		if(UGridPreviewComp)
		{
			
		}else
		{
			UE_LOG(LogTemp, Warning, TEXT("NO PREVIEW COMPONENT ATTACHED"));
		}
	}
	//VolumeRef.getcom
	//getcloserr..sdasa
}

/**
 * Given a Grid Surface, Place the Trap and occupy the grid surface.
 * @param GridSurface 
 */
void UGridInteractComponent::PlaceTrap (FGridSurface& GridSurface)
{
	
	GridSurface.bOccupied = true;
	UGridPlacementComponent* UGridPlaceComp = ActualVolumeRef->FindComponentByClass<UGridPlacementComponent>();
	if(UGridPlaceComp)
	{
		//place trap
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO PLACE COMPONENT ATTACHED"));
	}
}
