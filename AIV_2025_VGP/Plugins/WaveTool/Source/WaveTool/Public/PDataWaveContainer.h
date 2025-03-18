// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PDataWaveContainer.generated.h"

//TO DO: "GETTER"

// This enum is provisory and contains the "In game Waves".
// If you want a generated wave to spawn on the first round, insert 1 and so on...
UENUM(BlueprintType)
enum class ESpawnOrder : uint8
{
    Order1 UMETA(DisplayName = "1"),
    Order2 UMETA(DisplayName = "2"),
    Order3 UMETA(DisplayName = "3"),
    Order4 UMETA(DisplayName = "4"),
    Order5 UMETA(DisplayName = "5"),
    Order6 UMETA(DisplayName = "6"),
    Order7 UMETA(DisplayName = "7"),
    Order8 UMETA(DisplayName = "8"),
    Order9 UMETA(DisplayName = "9"),
    Order10 UMETA(DisplayName = "10"),
};

// This enum exist for easy input type
UENUM(BlueprintType)
enum class EEnemyTypes : uint8
{
    Melee UMETA(DisplayName = "Melee"),
    Ranged UMETA(DisplayName = "Ranged"),
    Tank UMETA(DisplayName = "Tank"),
};

// The next struct represents an enemy by its type and its point cost
USTRUCT(BlueprintType)
struct FInternalDumbEnemyType
{
    GENERATED_BODY()

    // This field indicates the enemy class 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    TSubclassOf<AActor> EnemyClass;

    // This field allow further customisation. if true you'll use the Cost and Greedy Process, else you can choose how many you want with MaxEnemies and MinEnemies 
    // (Note well, this values is part of a random, so if you want exactly 5 units just put 5 in both fields)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    bool bUseEnemyCost = true;

    // This field indicates the enemy Cost
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (EditCondition = "bUseEnemyCost"))
    int32 Cost = 0;    
    
    // This field indicates the meaximum enemy you want
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (EditCondition = "!bUseEnemyCost"))
    int32 MaxEnemies = 0;    
    
    // This field indicates the miminum enemy you want
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (EditCondition = "!bUseEnemyCost"))
    int32 MinEnemies = 0;

    // This field indicates the Type of the Enemy
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    EEnemyTypes EnemyType;
};

// This struct represents a wave setup
USTRUCT(BlueprintType)
struct FDumbWave
{
    GENERATED_BODY()

    // ID of the wave
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Settings")
    int32 WaveID;

    // Description of the wave (status and notes)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Settings")
    FString Description;

    // The points that the "Create Enemy Wave" has to spend
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Settings")
    int32 TotalPoints;

    // Types of enemies for the wave
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Settings")
    TArray<FInternalDumbEnemyType> EnemyTypes;
};

// This struct will be used to assign the waves to the desired spawner in the desired order
USTRUCT(BlueprintType)
struct WAVETOOL_API FWaveOrder
{
    GENERATED_BODY()

    // This array indicates the wave settings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Order")
    FDumbWave WaveSettings;

    // This field indicates the wave order for this wave
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Order")
    ESpawnOrder SpawnOrder;
};

// A Wave Container is basically a container of all waves of a level
UCLASS(BlueprintType)
class WAVETOOL_API UPDataWaveContainer : public UDataAsset
{
    GENERATED_BODY()

public:
    // ID of the wave container
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Container Settings")
    int32 WaveContainerID;

    // Description of the wave container (status and notes)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Container Settings")
    FString DescriptionOfContainer;

    // This array contains all the waves
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave Container Settings")
    TArray<FWaveOrder> WavesArray;
	
};
