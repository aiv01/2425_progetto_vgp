// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FormationDataAsset.h"
#include "GameFramework/Actor.h"
#include "PDataWaveContainer.h"
#include "WaveSpawner.h"
#include "SpawnInstruction.h"
#include "PWaveManager.generated.h"

class UDataAsset;
class AEnemySpawner;

//Delegate for UI or effects, events to notify wave status
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveCompleted, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDie, AActor*, EnemyDie);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnEnemySpawn, AActor*,EnemySpawn, bool, IsLeader,  FName, FormationName, int32, GroupID);
/*
*	bool bIsLeader =false;
	FName FormationName;
	int32 GroupID = -1;
 **/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndLevel);

UCLASS(Blueprintable)
class WAVETOOL_API APWaveManager : public AActor
{
	GENERATED_BODY()

public:
#pragma region Constructor
	APWaveManager();

#pragma endregion

	// Static instance pointer for the singleton pattern
	static APWaveManager* Instance;
	
	virtual void BeginPlay() override;

#pragma region UI Configurable Variables
	/** Wave Data Asset, contains all the wave settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Variables")
	UPDataWaveContainer* WaveDataAsset;
	
	/* Data asset for the spawn formation*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Formation")
	TArray<UFormationDataAsset*> FormationDataAssets;
	 
	/** Interval between waves (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Variables")
	float WaveInterval;
	
	/** Interval between waves (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Variables")
	float FormationSpawnFrequency;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Variables")
	float SpawnFrequency;
	
	/** Automatically starts the wave system at game start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Variables")
	bool bAutoStartWaveSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Formation")
	bool bUseFormation;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bEnableLogging = false;
#pragma endregion

#pragma region Wave System Variables
	/** List of enemy spawner locations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Variables")
	TArray<AWaveSpawner*> Spawners;
	
	/** Current wave index */
	int32 CurrentWaveIndex;

	/** Number of active enemies in the current wave */
	int32 ActiveEnemies;

	/** Timer to manage the interval between waves */
	FTimerHandle WaveTimerHandle;

	/** Array of waves sorted by order */
	TArray<FWaveOrder> WavesArray;

	/** Queue fo r current enemy */
	TArray<FSpawnInstruction> PendingSpawnQueue;

	/* Queue for pre computer the enemies positions*/
	TArray<FVector> PrecomputedSpawnPositions;

	float CachedTotalSpawnerWeight;
#pragma endregion

#pragma region Wave System Internal Functions
	/** Starts the next wave */
	void StartWave();
	
	/** Checks if the wave is completed */
	void CheckWaveCompletion();

	/** Called when a wave is finished */
	void OnWaveFinished();

	/** Handles the end of the level when all waves are completed */
	void EndLevel();
#pragma endregion

#pragma region Spawner Functions
	/* Main function to manage the spawning, detach if spawn formation or single one */
	void SpawnNextEnemy();
	void GenerateSpawnQueue();
	void CacheSpawnerWeights();
	//void CacheSpawnerWeights():
	/* Calculate a list of position using total enemy end distribution spawners*/
	TArray<FVector> ComputeSpawnPositions(int32 TotalEnemies);
	
	/* Calculate a list of position based on formation */
	TArray<FVector> ComputeSpawnPositionsFromComposition(const TArray<int32>& Composition);
	
	/* Using round-robin algorithm distribute the formation*/
	void ApplyFormationsToSpawnQueue_RoundRobin();
	
	/* Create the formation using roundRobin*/
	static TArray<FSpawnInstruction> GroupFormationInstructions_RoundRobin(
	const TMap<FName, TArray<TArray<FSpawnInstruction>>>& GroupsByFormation, const TArray<FName>& FormationOrder);
	
	/*Using for select a spawner based on weight spawners*/
	AWaveSpawner* GetRandomSpawnerByWeight() const;
	

	/*Using for select a spawner based on weight spawners - formation*/
	TArray<int32> ComputeSpawnQueueComposition() const;


#pragma endregion

#pragma region Delegates
	/** Event triggered when a wave starts */
	UPROPERTY(BlueprintAssignable, Category = "Wave System | Events")
	FOnWaveStarted OnWaveStarted;

	/** Event triggered when a wave is completed */
	UPROPERTY(BlueprintAssignable, Category = "Wave System | Events")
	FOnWaveCompleted OnWaveCompleted;

	/** Event triggered when an enemy is killed */
	UPROPERTY(BlueprintAssignable, Category = "Wave System | Events")
	FOnEnemyDie OnEnemyDie;

	/** Event triggered when an enemy spawn */
	UPROPERTY(BlueprintAssignable, Category = "Wave System | Events")
	FOnEnemySpawn OnEnemySpawn;

	/** Event triggered when level end */
	UPROPERTY(BlueprintAssignable, Category = "Wave System | Events")
	FOnEndLevel OnEndLevel;
	
#pragma region Event Binding Functions
	/** Binds a function to OnWaveStarted */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void BindOnWaveStarted(UObject* Object, FName FunctionName);

	/** Unbinds all functions from OnWaveStarted */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void UnbindOnWaveStarted(UObject* Object);

	/** Binds a function to OnWaveCompleted */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void BindOnWaveCompleted(UObject* Object, FName FunctionName);

	/** Unbinds all functions from OnWaveCompleted */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void UnbindOnWaveCompleted(UObject* Object);

	/** Binds a function to OnEnemyDie */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void BindOnEnemyDie(UObject* Object, FName FunctionName);

	/** Binds a function to OnEnemySpawn */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void BindOnEnemySpawn(UObject* Object, FName FunctionName);
	
	/** Unbinds all functions from OnEnemyDie */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void UnbindOnEnemyDie(UObject* Object);

	/** Unbinds all functions from OnEnemySpawn */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void UnbindOnEnemySpawn(UObject* Object);
	
	/** Binds a function to OnEnemyDie */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void BindOnEndLevel(UObject* Object, FName FunctionName);

	/** Unbinds all functions from OnEnemyDie */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void UnbindOnEndLevel(UObject* Object);
#pragma endregion


#pragma endregion

#pragma region Static Functions
	/** Static method to handle enemy death without needing a reference */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Functions")
	static void StaticHandleEnemyDie(AActor* EnemyDie);
#pragma endregion

#pragma region Wave System Functions
	// Function to initialize the manager after spawning
	UFUNCTION(BlueprintCallable, Category = "Wave System | Initialization")
	void InitializeWaveManager(UPDataWaveContainer* NewWaveData, const TArray<AWaveSpawner*>& NewSpawners, const float NewWaveInterval, const bool
	                           bNewAutoStartWaveSystem);
	/** Starts the wave system */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Functions")
	void StartWaveSystem();

	/** Sets new wave data */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Functions")
	void SetWaveData(UPDataWaveContainer* NewWaveData, bool bInEditorMode);

	/** Handles the death of an enemy */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Functions")
	void HandleEnemyDie(AActor* EnemyDie);

	/** Resets the wave system */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Functions")
	void ResetWaveSystem();

	/** Sets the interval between waves */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Functions")
	void SetWaveInterval(float NewInterval);
#pragma endregion

#pragma region Debug Functions - Variables
	/** Prints wave system info */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Debug")
	void PrintWaveInfo() const;

	/** Forces the next wave to start immediately */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Debug")
	void ForceNextWave();

	/** Kills all enemies in the current wave */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Debug")
	void KillAllEnemies();

	TArray<FString> PrecomputedSpawnerNames;
	
#pragma endregion

#pragma region Greedy
	UFUNCTION(BlueprintCallable, Category = "Wave Management")
	TMap<TSubclassOf<AActor>, int32> GenerateWave(int32 WavePoints, int32 PlayerCount, const TArray<FInternalDumbEnemyType>& AviableEnemies);

	int32 InternalRandom(int min, int max);
	/*	Example of Call
		TArray<TSubclassOf<AActor>> GenerateWave(1000, 4, EnemyList);
	*/
#pragma endregion

};
