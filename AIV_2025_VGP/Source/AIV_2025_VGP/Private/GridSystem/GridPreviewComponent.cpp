// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele
#include "GridSystem/GridPreviewComponent.h"

#include "Engine/StaticMeshActor.h"

/**
 * Show the Preview of the trap mesh
 * @param HitResult Hit Location
 * @param PositiveMeshMaterial
 * @param CloserSurface set the FGridSurface 
 */
void UGridPreviewComponent::ShowPreview(const FHitResult& HitResult, UMaterial* PositiveMeshMaterial, UMaterial* NegativeMeshMaterial, FGridSurface*& CloserSurface, const FName TrapRowName)
{
	
	// Get the closest surface from the hit result
	CloserSurface = GetCloserSurface(HitResult, TrapRowName);
	
	//check if surface is valid
	if(CloserSurface)
	{
		// Retrieve trap data from the volume using the TrapRowName
		FTrapData* TrapData = GridVolumeOwner->GetTrapData(TrapRowName);
		if(!TrapData)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trap not setted in datatable or name wrong"))
			return;
		}

		// If the preview mesh actor doesn't exist yet, create it
		if(!PreviewMesh)
		{
			TObjectPtr<UStaticMesh> StaticMesh = TrapData->Mesh;
			PreviewMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());

			// Setup mesh properties: mobility, collision, scale
			PreviewMesh->SetMobility(EComponentMobility::Movable);
			PreviewMesh->SetActorEnableCollision(false);
			PreviewMesh->SetActorScale3D(FVector{ GridVolumeOwner->GetHalfCellSize() / 50.f });
			
			// Assign static mesh from the trap data
			UStaticMeshComponent* MeshComponent = PreviewMesh->GetStaticMeshComponent();
			if (MeshComponent)
			{
				MeshComponent->SetStaticMesh(StaticMesh);
			}
		}

		// If the preview mesh is valid, set position, rotation, and materia
		if(PreviewMesh)
		{
			// Move the mesh to the surface position
			PreviewMesh->SetActorLocation(CloserSurface->Position);

			PreviewMesh->SetActorRotation(GetTrapRotation(CloserSurface->Orientation));

			// Determine if trap placement is valid or invalid, and apply appropriate material
			if((TrapData->Type.Contains(ETrapType::Floor) && CloserSurface->Orientation.Z != 1) ||
				(TrapData->Type.Contains(ETrapType::Wall) && CloserSurface->Orientation.X == 0 && CloserSurface->Orientation.Y == 0) ||
				(CloserSurface->bOccupied))
			{
				// Invalid trap placement (wrong surface type)
				if(!NegativeMeshMaterial)
				{
					UE_LOG(LogTemp, Error, TEXT("NEGATIVE MATERIAL NOT SET IN GRID INTERACT COMPONENT"));
					return;
				}
				
				PreviewMesh->GetComponentByClass<UStaticMeshComponent>()->SetMaterial(0, NegativeMeshMaterial);
				
			} else
			{
				// Valid trap placement
				if(!PositiveMeshMaterial)
				{
					UE_LOG(LogTemp, Error, TEXT("POSITIVE MATERIAL NOT SET IN GRID INTERACT COMPONENT"));
					return;
				}
				PreviewMesh->GetComponentByClass<UStaticMeshComponent>()->SetMaterial(0, PositiveMeshMaterial);
			}
		}
		
	}
	
}
/**
 * Destroys the preview mesh actor and clears the reference.
 */
void UGridPreviewComponent::ClearPreviewMesh()
{
	if(PreviewMesh)
	{
		PreviewMesh->Destroy();
		PreviewMesh = nullptr;
	}
}
