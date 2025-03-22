// Fill out your copyright notice in the Description page of Project Settings.


#include "PWaveManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "PDataWaveContainer.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#if  WITH_EDITOR
#include "Editor.h"
#endif

/*
 *Using UE_BUILD_SHIPPING /DEBUG for compile in different build (Debug, Development, Shipping)
 *
 **/

APWaveManager* APWaveManager::Instance = nullptr;

APWaveManager::APWaveManager() : WaveDataAsset(nullptr), WaveInterval(0), SpawnFrequency(0),
                                 bAutoStartWaveSystem(false), CurrentWaveIndex(0),
                                 ActiveEnemies(0)
{
	//Disable tick 
	PrimaryActorTick.bCanEverTick = false;
}

void APWaveManager::BeginPlay()
{
	Super::BeginPlay();
	Instance = this; //Singleton
	if (!WaveDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave Manager: Data Asset not assigned."));
		return;
	}
#if UE_BUILD_DEVELOPMENT
	if (bAutoStartWaveSystem)
	{
		UE_LOG(LogTemp, Log, TEXT("Wave Manager: Starting wave system."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Wave Manager: Waiting for manual wave start."));
	}
#endif
	if (bAutoStartWaveSystem)
	{
		StartWaveSystem();
	}
}


#pragma region Debug Functions
void APWaveManager::PrintWaveInfo() const
{
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Current Wave Index: %d"), CurrentWaveIndex);
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Total Waves Loaded: %d"), WavesArray.Num());
}

void APWaveManager::ForceNextWave()
{
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Forcing next wave."));
	StartWave();
}

void APWaveManager::KillAllEnemies()
{
	ActiveEnemies = 0;
	CheckWaveCompletion();
}
#pragma endregion

#pragma region Spawner Functions
FVector APWaveManager::GetSpawnerLocationByWeight() const
{
	if (Spawners.Num() == 0)
	{
		return GetActorLocation();
	}

	float TotalWeight = 0.0f;
	for (const AWaveSpawner* Spawner : Spawners)
	{
		if (Spawner)
		{
			TotalWeight += Spawner->SpawnWeight;
		}
	}

	float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);
	for (const AWaveSpawner* Spawner : Spawners)
	{
		if (Spawner)
		{
			RandomWeight -= Spawner->SpawnWeight;
			if (RandomWeight <= 0.0f)
			{
				return Spawner->GetRandomSpawnLocation();
			}
		}
	}
	return Spawners[0] ? Spawners[0]->GetRandomSpawnLocation() : GetActorLocation();
}

void APWaveManager::SpawnNextEnemy()
{
	if (PendingSpawnQueue.Num() == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
#if UE_BUILD_DEVELOPMENT
		UE_LOG(LogTemp, Log, TEXT("WaveManager: Finished spawning all enemies for wave %d."), CurrentWaveIndex + 1);
#endif
		return;
	}
    
	//Get end remove first instruction
	FSpawnInstruction Instruction = PendingSpawnQueue[0];
	PendingSpawnQueue.RemoveAt(0);
    FVector Position = PrecomputedSpawnPositions[0];
    PrecomputedSpawnPositions.RemoveAt(0);
	
	//Use the first element of instruction end position
	FVector SpawnLocation = Position;
	FRotator SpawnRotation = GetActorRotation();
    
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
	AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(Instruction.EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (SpawnedEnemy)
	{
		OnEnemySpawn.Broadcast(SpawnedEnemy);
#if UE_BUILD_DEVELOPMENT
		//UE_LOG(LogTemp, Log, TEXT("WaveManager: Spawned enemy '%s' of type '%s'."), *SpawnedEnemy->GetName(), *Instruction.EnemyClass->GetName());
#endif
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: Failed to spawn enemy of type '%s'."), *Instruction.EnemyClass->GetName());
	}
}

TArray<FVector> APWaveManager::ComputeSpawnPositions(int32 TotalEnemies) const
{
    TArray<FVector> SpawnPositions;
	
    if (Spawners.Num() == 0)
    {
        for (int32 i = 0; i < TotalEnemies; i++)
        {
            SpawnPositions.Add(GetActorLocation());
        }
        return SpawnPositions;
    }

    float TotalWeight = 0.0f;
    for (const AWaveSpawner* Spawner : Spawners)
    {
        if (Spawner)
        {
            TotalWeight += Spawner->SpawnWeight;
        }
    }

    TArray<int32> Distribution;
    Distribution.Init(0, Spawners.Num());
    int32 Accumulated = 0;
    for (int32 i = 0; i < Spawners.Num(); i++)
    {
        if (Spawners[i])
        {
            int32 Count = FMath::FloorToInt((Spawners[i]->SpawnWeight / TotalWeight) * TotalEnemies);
            Distribution[i] = Count;
            Accumulated += Count;
        }
    }

    int32 Remainder = TotalEnemies - Accumulated;
    if (Remainder > 0 && Spawners.Num() > 0)
    {
        Distribution[Spawners.Num()-1] += Remainder;
    }
	//Get spawn points
    for (int32 i = 0; i < Spawners.Num(); i++)
    {
        if (Spawners[i])
        {
            for (int32 j = 0; j < Distribution[i]; j++)
            {
                
                FVector Pos = Spawners[i]->GetRandomSpawnLocation();
                SpawnPositions.Add(Pos);
            }
        }
    }
    for (int32 i = 0; i < SpawnPositions.Num(); i++)
    {
        int32 SwapIndex = FMath::RandRange(0, SpawnPositions.Num()-1);
        SpawnPositions.Swap(i, SwapIndex);
    }
    
    return SpawnPositions;
}
#pragma endregion

#pragma region Greedy
TMap<TSubclassOf<AActor>, int32> APWaveManager::GenerateWave(const int32 WavePoints, const int32 PlayerCount, const TArray<FInternalDumbEnemyType>& AviableEnemies)
{
	// Creazione delle variabili per il processo
	TMap<TSubclassOf<AActor>, int32> SelectedEnemies;// Contiene gli nemici selezionati
	TMap<EEnemyTypes, int32> EnemyCount;// Conta quanti nemici di ogni tipo sono gi� stati scelti
	TMap<EEnemyTypes, int32> PointBuyCount; // Conta quanti nemici sono stati aggiunti SOLO con Point Buy
	int32 RemainingPoints = WavePoints;// Punti rimanenti per l'acquisto dei nemici
	int32 MaxTanks = PlayerCount;// Per ora, il numero di tank � limitato ai giocatori

	// Inizializza il conteggio delle categorie
	EnemyCount.Add(EEnemyTypes::Melee, 0);
	EnemyCount.Add(EEnemyTypes::Ranged, 0);
	EnemyCount.Add(EEnemyTypes::Tank, 0);

	// Inizializza il conteggio delle categorie riservate al point buy
	PointBuyCount.Add(EEnemyTypes::Melee, 0);
	PointBuyCount.Add(EEnemyTypes::Ranged, 0);
	PointBuyCount.Add(EEnemyTypes::Tank, 0);

	// Ordina gli nemici dal costo pi� basso al pi� alto
	TArray<FInternalDumbEnemyType> SortedEnemies = AviableEnemies;
	SortedEnemies.Sort([](const FInternalDumbEnemyType& A, const FInternalDumbEnemyType& B) { return A.Cost > B.Cost; });


	auto AddEnemy = [&](const FInternalDumbEnemyType& Enemy, int32 Count, bool bIsPointBuy)
		{
			if (Count <= 0) return; // Evita aggiunte inutili

			SelectedEnemies.FindOrAdd(Enemy.EnemyClass) += Count;
			EnemyCount[Enemy.EnemyType] += Count;
			if (bIsPointBuy) // Se � stato acquistato con point buy, aggiorniamo PointBuyCount
			{
				PointBuyCount[Enemy.EnemyType] += Count;
			}
		};

	
	for (const FInternalDumbEnemyType& Enemy : SortedEnemies)
	{
		if (!Enemy.bUseEnemyCost)  // Caso Min Max
		{
			int32 SpawnCount = InternalRandom(Enemy.MinEnemies, Enemy.MaxEnemies);

			AddEnemy(Enemy, SpawnCount, false);
		}
	}

	while (RemainingPoints > 0)
	{
		bool bAddedEnemy = false;

		for (const FInternalDumbEnemyType& Enemy : SortedEnemies)
		{
			if (!Enemy.bUseEnemyCost || Enemy.Cost > RemainingPoints)
			{
				continue; // Salta i nemici Min-Max e quelli troppo costosi
			}

			// Controlla se il numero massimo di Tank � stato raggiunto
			if (Enemy.EnemyType == EEnemyTypes::Tank && EnemyCount[EEnemyTypes::Tank] >= MaxTanks)
			{
				continue;
			}

			bool bOnlyMelee = (PointBuyCount[EEnemyTypes::Ranged] == 0) && Enemy.bUseEnemyCost;
			bool bOnlyRanged = (PointBuyCount[EEnemyTypes::Melee] == 0) && Enemy.bUseEnemyCost;
		
			if (!bOnlyMelee && !bOnlyRanged)
			{	
				if ((Enemy.EnemyType == EEnemyTypes::Melee && PointBuyCount[EEnemyTypes::Melee] > PointBuyCount[EEnemyTypes::Ranged] + 1) ||
					(Enemy.EnemyType == EEnemyTypes::Ranged && PointBuyCount[EEnemyTypes::Ranged] > PointBuyCount[EEnemyTypes::Melee] + 1))
				{
					continue;
				}
			}
			// Aggiunge il nemico e scala i punti
			AddEnemy(Enemy, 1, true);
			RemainingPoints -= Enemy.Cost;
			bAddedEnemy = true;
		}

		// Se nessun nemico � stato aggiunto, il loop si ferma
		if (!bAddedEnemy)
		{
			break;
		}
	}

	// Restituisce la mappa con gli nemici selezionati e la loro quantit�
	return SelectedEnemies;
}
int32 APWaveManager::InternalRandom(int Min, int Max)
{
	UE_LOG(LogTemp, Warning, TEXT("Generating random number between %d and %d"), Min, Max);
	return FMath::RandRange(Min, Max);
}
#pragma endregion

#pragma region Wave System Internal Functions
void APWaveManager::StartWave()
{
	if (CurrentWaveIndex >= WavesArray.Num())
	{
#if UE_BUILD_DEVELOPMENT
		UE_LOG(LogTemp, Log, TEXT("WaveManager: All waves completed."));
#endif
		EndLevel();
		return;
	}

#if UE_BUILD_DEVELOPMENT
	FWaveOrder CurrentWave = WavesArray[CurrentWaveIndex];
	/*UE_LOG(LogTemp, Log, TEXT("WaveManager: Starting wave %d with %d wave settings."), CurrentWaveIndex + 1, CurrentWave.WaveSettings.Num());*/
#endif
	NewStartWave();
	//Notify to the listener 
	OnWaveStarted.Broadcast(CurrentWaveIndex + 1);
}

void APWaveManager::NewStartWave()
{
	if (CurrentWaveIndex >= WavesArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: No valid wave at index %d."), CurrentWaveIndex);
		return;
	}
	//Get current wave
	const FWaveOrder& CurrentWaveOrder = WavesArray[CurrentWaveIndex];

#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Spawning enemies for wave %d."), CurrentWaveIndex + 1);
#endif
	
	//Get the pair <enemy, number to spawn>
	ActiveEnemies = 0;
	int PoolOfPoints = WaveDataAsset->WavesArray[0].WaveSettings.TotalPoints;
	TArray<FInternalDumbEnemyType> Enemies = WaveDataAsset->WavesArray[0].WaveSettings.EnemyTypes;
	TMap<TSubclassOf<AActor>, int32> EnemyList = GenerateWave(PoolOfPoints, 3, Enemies);
	
	for (const TPair<TSubclassOf<AActor>, int32>& Pair : EnemyList)
	{
		UE_LOG(LogTemp, Log, TEXT("WaveManager: For enemy class '%s', spawn count is %d"), *Pair.Key->GetName(), Pair.Value);
		ActiveEnemies += Pair.Value;
	}

	//Queue for spawn
	PendingSpawnQueue.Empty();
	for (const TPair<TSubclassOf<AActor>, int32>& Pair : EnemyList)
	{
		for (int32 i = 0; i < Pair.Value; ++i)
		{
			FSpawnInstruction Instruction;
			Instruction.EnemyClass = Pair.Key;
			Instruction.Count = 1;  //A single Spawn
			PendingSpawnQueue.Add(Instruction);
		}
	}
	//Shuffle
	for (int32 i = PendingSpawnQueue.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		PendingSpawnQueue.Swap(i, j);
	}
	PrecomputedSpawnPositions = ComputeSpawnPositions(ActiveEnemies);
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this, &APWaveManager::SpawnNextEnemy, SpawnFrequency, true);
	}
}

void APWaveManager::CheckWaveCompletion()
{
	if (ActiveEnemies <= 0)
	{
#if UE_BUILD_DEVELOPMENT
		UE_LOG(LogTemp, Log, TEXT("WaveManager: Wave %d completed."), CurrentWaveIndex + 1);
#endif
		OnWaveFinished();
	}
}

void APWaveManager::OnWaveFinished()
{
	//Notify to the listener
	OnWaveCompleted.Broadcast(CurrentWaveIndex + 1);

	CurrentWaveIndex++;

	if (CurrentWaveIndex < WavesArray.Num())
	{
#if UE_BUILD_DEVELOPMENT
		UE_LOG(LogTemp, Log, TEXT("WaveManager: Scheduling next wave in %f seconds."), WaveInterval);
#endif
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this, &APWaveManager::StartWave, WaveInterval, false);
		}
	}
	else
	{
#if UE_BUILD_DEVELOPMENT
		UE_LOG(LogTemp, Log, TEXT("WaveManager: All waves completed."));
#endif
		EndLevel();
	}
}

void APWaveManager::EndLevel()
{
	OnEndLevel.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Ending level."));
}
#pragma endregion

#pragma region Event Binding Functions
void APWaveManager::BindOnWaveStarted(UObject* Object, FName FunctionName)
{
	if (!Object) return;
	if (FunctionName.IsNone()) return;

	FScriptDelegate Delegate;
	Delegate.BindUFunction(Object, FunctionName);
	OnWaveStarted.AddUnique(Delegate);
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Function '%s' bound to OnWaveStarted."), *FunctionName.ToString());
#endif

}

void APWaveManager::UnbindOnWaveStarted(UObject* Object)
{
	if (!Object) return;

	OnWaveStarted.RemoveAll(Object);
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Removed all OnWaveStarted bindings for object %s."), *Object->GetName());
#endif
}

void APWaveManager::BindOnWaveCompleted(UObject* Object, FName FunctionName)
{
	if (!Object) return;
	if (FunctionName.IsNone()) return;

	FScriptDelegate Delegate;
	Delegate.BindUFunction(Object, FunctionName);
	OnWaveCompleted.AddUnique(Delegate);

#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Function '%s' bound to OnWaveCompleted."), *FunctionName.ToString());
#endif
}

void APWaveManager::UnbindOnWaveCompleted(UObject* Object)
{
	if (!Object) return;

	OnWaveCompleted.RemoveAll(Object);
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Removed all OnWaveCompleted bindings for object %s."), *Object->GetName());
#endif
}

void APWaveManager::BindOnEnemyDie(UObject* Object, FName FunctionName)
{
	if (!Object) return;
	if (FunctionName.IsNone()) return;

	FScriptDelegate Delegate;
	Delegate.BindUFunction(Object, FunctionName);
	OnEnemyDie.AddUnique(Delegate);

#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Function '%s' bound to OnEnemyDie."), *FunctionName.ToString());
#endif

}

void APWaveManager::UnbindOnEnemyDie(UObject* Object)
{
	if (!Object) return;

	OnEnemyDie.RemoveAll(Object);
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Removed all OnEnemyDie bindings for object %s."), *Object->GetName());
#endif
}

void APWaveManager::BindOnEnemySpawn(UObject* Object, FName FunctionName)
{
	if (!Object)
	{
		return;
	}
	if (FunctionName.IsNone())
	{
		return;
	}
	FScriptDelegate Delegate;
	Delegate.BindUFunction(Object, FunctionName);
	OnEnemySpawn.AddUnique(Delegate);

#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Function '%s' bound to OnEnemySpawn."), *FunctionName.ToString());
#endif

}

void APWaveManager::UnbindOnEnemySpawn(UObject* Object)
{
	if (!Object)
	{
		return;
	}
	OnEnemySpawn.RemoveAll(Object);
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Removed all OnEnemySpawn bindings for object %s."), *Object->GetName());
#endif
}

void APWaveManager::BindOnEndLevel(UObject* Object, FName FunctionName)
{
	if (!Object) return;
	if (FunctionName.IsNone()) return;

	FScriptDelegate Delegate;
	Delegate.BindUFunction(Object, FunctionName);
	OnEndLevel.AddUnique(Delegate);

#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Function '%s' bound to OnEndLevel."), *FunctionName.ToString());
#endif

}

void APWaveManager::UnbindOnEndLevel(UObject* Object)
{
	if (!Object) return;

	OnEnemyDie.RemoveAll(Object);
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Removed all OnEndLevel bindings for object %s."), *Object->GetName());
#endif
}
#pragma endregion

#pragma region Static Functions
void APWaveManager::StaticHandleEnemyDie(AActor* EnemyDie)
{
	if (Instance)
	{
		Instance->HandleEnemyDie(EnemyDie);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: StaticHandleEnemyDie called but no WaveManager instance exists."));
	}
}
#pragma endregion

#pragma region Wave System Functions
void APWaveManager::InitializeWaveManager(UPDataWaveContainer* NewWaveData,
	const TArray<AWaveSpawner*>& NewSpawners, const float NewWaveInterval, const bool bNewAutoStartWaveSystem)
{
	if (!NewWaveData)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: Invalid Data Asset provided."));
		return;
	}

	WaveDataAsset = NewWaveData;
	Spawners = NewSpawners;
	this->WaveInterval = WaveInterval;
	this->bAutoStartWaveSystem = bNewAutoStartWaveSystem;
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Initialized with new data. WaveInterval: %f, Spawners: %d"), WaveInterval, Spawners.Num());
#endif
}

void APWaveManager::SetWaveData(UPDataWaveContainer* NewWaveData, bool bInEditorMode)
{
	if (!NewWaveData)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: Tried to set an invalid Wave Data Asset."));
		return;
	}
	WaveDataAsset = NewWaveData;
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Wave Data Asset updated successfully."));
#endif
	if (!bInEditorMode)
	{
#if UE_BUILD_DEVELOPMENT
		UE_LOG(LogTemp, Log, TEXT("WaveManager: Restarting wave system with new data."));
#endif
		ResetWaveSystem();
		StartWaveSystem();
	}
}

void APWaveManager::StartWaveSystem()
{
	if (!WaveDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: No Wave Data Asset assigned!"));
		return;
	}
	if (WaveDataAsset->WavesArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: No waves found in the Wave Data Asset."));
		return;
	}

	//Using Espawn order
	WavesArray = WaveDataAsset->WavesArray;
	WavesArray.Sort([](const FWaveOrder& A, const FWaveOrder& B)
		{
			return A.SpawnOrder < B.SpawnOrder;
		});

	CurrentWaveIndex = 0;
	if (GetWorld())
	{
		// Using for handle the bind before the start. 
		float InitialDelay = 1.0f;
		GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this, &APWaveManager::StartWave, InitialDelay, false);
	}
}

void APWaveManager::HandleEnemyDie(AActor* EnemyDie)
{
	if (!EnemyDie)
	{
#if UE_BUILD_DEVELOPMENT
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: HandleEnemyDie called with a null enemy."));
#endif
		return;
	}
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Enemy died: %s"), *EnemyDie->GetName());
#endif

	//Notify to the listener
	OnEnemyDie.Broadcast(EnemyDie);
	ActiveEnemies--;
	CheckWaveCompletion();
}


void APWaveManager::ResetWaveSystem()
{
	CurrentWaveIndex = 0;
	ActiveEnemies = 0;
	WavesArray.Empty();
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
	}
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Wave system has been reset."));
#endif
}

void APWaveManager::SetWaveInterval(float NewInterval)
{
	if (NewInterval <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: Invalid Wave Interval! Setting default value (5 seconds)."));
		NewInterval = 5.0f; //Default 
	}

	WaveInterval = NewInterval;
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Wave interval set to %f seconds."), WaveInterval);
#endif
}
#pragma endregion

