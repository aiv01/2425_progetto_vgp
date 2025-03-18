#pragma once

#include "CoreMinimal.h"
#include "SpawnInstruction.generated.h"

USTRUCT()
struct WAVETOOL_API FSpawnInstruction
{
	GENERATED_BODY()
	
	TSubclassOf<AActor> EnemyClass;
	int32 Count;
};
