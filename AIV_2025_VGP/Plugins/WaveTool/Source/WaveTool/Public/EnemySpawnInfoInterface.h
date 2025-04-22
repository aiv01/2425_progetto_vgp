// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemySpawnInfoInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UEnemySpawnInfoInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WAVETOOL_API IEnemySpawnInfoInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="EnemySpawn")
	void SetSpawnInfo(bool bIsLeader, FName FormationName, int32 GroupID);
public:
};
