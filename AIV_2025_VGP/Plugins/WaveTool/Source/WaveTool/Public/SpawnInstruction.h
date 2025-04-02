#pragma once

#include "CoreMinimal.h"
#include "PDataWaveContainer.h"
#include "SpawnInstruction.generated.h"

USTRUCT()
struct WAVETOOL_API FSpawnInstruction
{
	GENERATED_BODY()
	
	TSubclassOf<AActor> EnemyClass;
	int32 Count;
	//Formation
	EEnemyTypes EnemyType;
	bool bIsLeader =false;
	FName FormationName;
	int32 GroupID = -1;
	FVector SpawnPosition = FVector::ZeroVector;
};
