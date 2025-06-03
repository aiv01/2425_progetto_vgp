// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDataWaveContainer.h"
#include "Engine/DataAsset.h"
#include "FormationDataAsset.generated.h"

USTRUCT(BlueprintType)
struct WAVETOOL_API FEnemyFormationEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EEnemyTypes EnemyType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	int32 Quantity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	bool bIsLeader;
};

UCLASS(BlueprintType)
class WAVETOOL_API UFormationDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	FName FormationName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	TArray<FEnemyFormationEntry> FormationEntries;

	virtual void BeginDestroy() override;
};
