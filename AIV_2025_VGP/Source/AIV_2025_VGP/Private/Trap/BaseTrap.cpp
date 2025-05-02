// Fill out your copyright notice in the Description page of Project Settings.


#include "Trap/BaseTrap.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


// Sets default values
ABaseTrap::ABaseTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    Box->SetupAttachment(RootComponent);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    Mesh->SetupAttachment(Box);

    Damage = 20.f;
    TrapCost = 100;
}

// Called when the game starts or when spawned
void ABaseTrap::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseTrap::ActivateTrap()
{
    if (!bCanActivate)
        return;

    TArray<AActor*> OverlappingActors;
    Box->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        UGameplayStatics::ApplyDamage(Actor, Damage, GetInstigatorController(), this, DamageType);
    }

    ResetTrap();
}

void ABaseTrap::ResetTrap()
{
    bCanActivate = false;
 
    GetWorld()->GetTimerManager().SetTimer(
        ResetTimerHandle,
        this,
        &ABaseTrap::EnableTrap,
        CooldownTime,
        false
    );
}

void ABaseTrap::EnableTrap()
{
    bCanActivate = true;
}