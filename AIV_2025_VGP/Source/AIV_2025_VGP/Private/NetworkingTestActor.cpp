// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkingTestActor.h"
#include "OnlineSubsystem.h"
#include "PNetworking.h"

// Sets default values
ANetworkingTestActor::ANetworkingTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANetworkingTestActor::BeginPlay()
{
	Super::BeginPlay();

	if (FPNetworkingModule::GetOnlineSubsystemReference())
	{
#pragma region LOGS
		//UE_LOG(LogTemp, Warning, TEXT("Initialized!"));
		//UE_LOG(LogTemp, Warning, TEXT("App ID: %s"), *FPNetworkingModule::GetOnlineSubsystemReference()->GetAppId());
#pragma endregion LOGS
	}
}

// Called every frame
void ANetworkingTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

