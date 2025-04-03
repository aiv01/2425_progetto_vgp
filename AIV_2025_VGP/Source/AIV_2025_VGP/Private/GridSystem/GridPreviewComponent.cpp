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
 * @param MeshMaterial
 * @param CloserSurface set the FGridSurface 
 */
void UGridPreviewComponent::ShowPreview(const FHitResult& HitResult, UMaterial* MeshMaterial, FGridSurface*& CloserSurface, const FName TrapRowName)
{
	
	//set closer surface ref
	CloserSurface = GetCloserSurface(HitResult, TrapRowName);
	//check if surface is valid
	if(CloserSurface)
	{
		if(!MeshMaterial)
		{
			UE_LOG(LogTemp, Error, TEXT("MATERIAL NOT SET IN GRID INTERACT COMPONENT"));
			return;
		}
		FTrapData* TrapData = GridVolumeOwner->GetTrapData(TrapRowName);

		if(!TrapData)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trap not setted in datatable or name wrong"))
			return;
		}
		if(TrapData->Type.Contains(ETrapType::Floor) && CloserSurface->Orientation.Z != 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("GRID NOT ON THE FLOOR"))
			return;
		}
		if(TrapData->Type.Contains(ETrapType::Wall) && CloserSurface->Orientation.X != 0 && CloserSurface->Orientation.Y != 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("GRID NOT ON THE WALL"))
			return;
		}
		
		if(!PreviewMesh)
		{
			TObjectPtr<UStaticMesh> StaticMesh = TrapData->Mesh;
			PreviewMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
			PreviewMesh->SetMobility(EComponentMobility::Movable);
			UStaticMeshComponent* MeshComponent = PreviewMesh->GetStaticMeshComponent();
			if (MeshComponent)
			{
				MeshComponent->SetStaticMesh(StaticMesh);
			}
			
		}
		if(PreviewMesh)
		{
			
			PreviewMesh->SetActorLocation(CloserSurface->Position);
			PreviewMesh->GetComponentByClass<UStaticMeshComponent>()->SetMaterial(0, MeshMaterial);
			
		}
		
		//DrawDebug(CloserSurface);
		//Place preview Turret
	}
	
}

void UGridPreviewComponent::ClearPreviewMesh()
{
	PreviewMesh->Destroy();
	PreviewMesh = nullptr;
}
