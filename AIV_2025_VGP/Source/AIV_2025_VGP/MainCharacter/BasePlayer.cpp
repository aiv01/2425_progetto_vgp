// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayer.h"


// Sets default values
ABasePlayer::ABasePlayer()
{

	FString SkeletalMeshPath = "/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny";
	USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *SkeletalMeshPath);
	if (SkeletalMesh)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMesh);
		UE_LOG(LogTemp, Warning, TEXT("BasePlayer::ABasePlayer mesh found!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BasePlayer::ABasePlayer mesh NOT found!"));
	}
}

// Called when the game starts or when spawned
void ABasePlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

