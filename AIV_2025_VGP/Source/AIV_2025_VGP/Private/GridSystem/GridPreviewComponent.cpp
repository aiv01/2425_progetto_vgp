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
	
	//set closer surface ref
	CloserSurface = GetCloserSurface(HitResult, TrapRowName);
	//check if surface is valid
	if(CloserSurface)
	{
		FTrapData* TrapData = GridVolumeOwner->GetTrapData(TrapRowName);
		if(!TrapData)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trap not setted in datatable or name wrong"))
			return;
		}

		if(!PreviewMesh)
		{
			TObjectPtr<UStaticMesh> StaticMesh = TrapData->Mesh;
			PreviewMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
			PreviewMesh->SetMobility(EComponentMobility::Movable);
			PreviewMesh->SetActorEnableCollision(false);
			PreviewMesh->SetActorScale3D(FVector{ GridVolumeOwner->GetHalfCellSize() / 50.f });
			
			
			
			UStaticMeshComponent* MeshComponent = PreviewMesh->GetStaticMeshComponent();
			if (MeshComponent)
			{
				MeshComponent->SetStaticMesh(StaticMesh);
			}
			
		}
		
		if(PreviewMesh)
		{
			PreviewMesh->SetActorLocation(CloserSurface->Position);
			
			FVector Forward = CloserSurface->Orientation.GetSafeNormal(); // Assicura che sia un vettore unitario
			FVector Right = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal(); // Ortogonale a Forward
			FVector Up = FVector::CrossProduct(Forward, Right); // Ortogonale a entrambi
			FMatrix RotationMatrix = FMatrix(Forward, Right, Up, FVector::ZeroVector);
			FRotator TargetRotation = RotationMatrix.Rotator();	
			TargetRotation.Pitch -= 90.0f;
			PreviewMesh->SetActorRotation(TargetRotation);
			
			if((TrapData->Type.Contains(ETrapType::Floor) && CloserSurface->Orientation.Z != 1) ||
				(TrapData->Type.Contains(ETrapType::Wall) && CloserSurface->Orientation.X != 0 && CloserSurface->Orientation.Y != 0))
			{
				if(!NegativeMeshMaterial)
				{
					UE_LOG(LogTemp, Error, TEXT("NEGATIVE MATERIAL NOT SET IN GRID INTERACT COMPONENT"));
					return;
				}
				
				PreviewMesh->GetComponentByClass<UStaticMeshComponent>()->SetMaterial(0, NegativeMeshMaterial);
				
			} else
			{
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

void UGridPreviewComponent::ClearPreviewMesh()
{
	PreviewMesh->Destroy();
	PreviewMesh = nullptr;
}
