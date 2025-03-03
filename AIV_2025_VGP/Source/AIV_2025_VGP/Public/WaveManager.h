// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataWaveContainer.h"
#include "WaveManager.generated.h"

class UDataAsset;
class AEnemySpawner;

//Delegate for UI or effects, events to notify wave status
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveCompleted, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDie, AActor*, EnemyDie);



UCLASS(Blueprintable)
class AIV_2025_VGP_API AWaveManager : public AActor
{
	GENERATED_BODY()

public:
#pragma region Constructor
	//Default
	AWaveManager();
	//Custom constructor for all the param
	//explicit AWaveManager(UDataWaveContainer* NewWaveData, const TArray<FTransform>& SpawnerLocations, const float WaveInterval, const bool AutoStartWaveSystem);
#pragma endregion

	// Static instance pointer for the singleton pattern
	static AWaveManager* Instance;

public:
	virtual void BeginPlay() override;

#pragma region UI Configurable Variables
	/** Wave Data Asset, contains all the wave settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Variables")
	UDataWaveContainer* WaveDataAsset;

	/** Interval between waves (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Variables")
	float WaveInterval;

	/** Automatically starts the wave system at game start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Variables")
	bool bAutoStartWaveSystem;
#pragma endregion

#pragma region Wave System Variables
	/** List of enemy spawner locations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System | Variables")
	TArray<FTransform> EnemySpawners;

	/** Current wave index */
	int32 CurrentWaveIndex;

	/** Number of active enemies in the current wave */
	int32 ActiveEnemies;

	/** Timer to manage the interval between waves */
	FTimerHandle WaveTimerHandle;

	/** Array of waves sorted by order */
	TArray<FWaveOrder> WavesArray;
#pragma endregion

#pragma region Wave System Internal Functions
	/** Starts the next wave */
	void StartWave();

	/** Starts the next wave */
	void NewStartWave();

	/** Spawns enemies for the current wave */
	void SpawnWave();

	/** Checks if the wave is completed */
	void CheckWaveCompletion();

	/** Called when a wave is finished */
	void OnWaveFinished();

	/** Handles the end of the level when all waves are completed */
	void EndLevel();
#pragma endregion

public:

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

	/** Unbinds all functions from OnEnemyDie */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Events")
	void UnbindOnEnemyDie(UObject* Object);
#pragma endregion


#pragma endregion
#pragma region Static Functions
	/** Static method to handle enemy death without needing a reference */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Functions")
	static void StaticHandleEnemyDie(AActor* EnemyDie);
#pragma endregion
#pragma region Public Wave Control Functions
	// Function to initialize the manager after spawning
	UFUNCTION(BlueprintCallable, Category = "Wave System | Initialization")
	void InitializeWaveManager(UDataWaveContainer* NewWaveData, const TArray<FTransform>& SpawnerLocations, const float NewWaveInterval, const bool bNewAutoStartWaveSystem);
	/** Starts the wave system */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Functions")
	void StartWaveSystem();

	/** Sets new wave data */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Functions")
	void SetWaveData(UDataWaveContainer* NewWaveData, bool bInEditorMode);

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

#pragma region Debug Functions
	/** Prints wave system info */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Debug")
	void PrintWaveInfo() const;

	/** Forces the next wave to start immediately */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Debug")
	void ForceNextWave();

	/** Kills all enemies in the current wave */
	UFUNCTION(BlueprintCallable, Category = "Wave System | Debug")
	void KillAllEnemies();
#pragma endregion

#pragma region Greedy
	UFUNCTION(BlueprintCallable, Category = "Wave Management")
	TMap<TSubclassOf<AActor>, int32> GenerateWave(int32 WavePoints, int32 PlayerCount, const TArray<FInternalDumbEnemyType>& AviableEnemies);

	/*	Example of Call
		TArray<TSubclassOf<AActor>> GenerateWave(1000, 4, EnemyList);
	*/
#pragma endregion
};
