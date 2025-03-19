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

// Called when the game starts or when spawned
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
	//TODO: Create event to bind the event kill
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

#if UE_BUILD_DEVELOPMENT
	for (const TPair<TSubclassOf<AActor>, int32>& Pair : EnemyList)
	{
		UE_LOG(LogTemp, Log, TEXT("WaveManager: For enemy class '%s', spawn count is %d"), *Pair.Key->GetName(), Pair.Value);
	}
#endif
	//Queue for spawn
	PendingSpawnQueue.Empty();
	int32 TotalSpawns = 0;
	for (const TPair<TSubclassOf<AActor>, int32>& Pair : EnemyList)
	{
		FSpawnInstruction Instruction;
		Instruction.EnemyClass = Pair.Key;
		Instruction.Count = Pair.Value;
		PendingSpawnQueue.Add(Instruction);
		TotalSpawns += Pair.Value;
	}
	ActiveEnemies = TotalSpawns;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this, &APWaveManager::SpawnNextEnemy, SpawnFrequency, true);
	}
}
void APWaveManager::SpawnNextEnemy()
{
    //Queue Empty
    if (PendingSpawnQueue.Num() == 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
#if UE_BUILD_DEVELOPMENT
        UE_LOG(LogTemp, Log, TEXT("WaveManager: Finished spawning all enemies for wave %d."), CurrentWaveIndex + 1);
#endif
        return;
    }
	//Get first instruction
    FSpawnInstruction& Instruction = PendingSpawnQueue[0];
    if (Instruction.Count > 0)
    {
        FVector SpawnLocation = GetSpawnerLocationByWeight();
        FRotator SpawnRotation = GetActorRotation();
        
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        
        AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(Instruction.EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (SpawnedEnemy)
        {
#if UE_BUILD_DEVELOPMENT
            UE_LOG(LogTemp, Log, TEXT("WaveManager: Spawned enemy '%s' of type '%s'."), *SpawnedEnemy->GetName(), *Instruction.EnemyClass->GetName());
        	OnEnemySpawn.Broadcast(SpawnedEnemy);
#endif
            Instruction.Count--;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("WaveManager: Failed to spawn enemy of type '%s'."), *Instruction.EnemyClass->GetName());
        }
    }
    if (Instruction.Count <= 0)
    {
        PendingSpawnQueue.RemoveAt(0);
    }
}


TMap<TSubclassOf<AActor>, int32> APWaveManager::GenerateWave(const int32 WavePoints, const int32 PlayerCount, const TArray<FInternalDumbEnemyType>& AviableEnemies)
{
	//creating the variable for the process
	TMap<TSubclassOf<AActor>, int32> SelectedEnemies;	//an array to contains the enemies to return
	TMap<EEnemyTypes, int32> EnemyCount;			//a map usefull to count how many enemies of wich type are already selected
	int32 RemainingPoints = WavePoints;				//the points spent by the Greedy to buy
	int32 MaxTanks = PlayerCount;					//for now, number of players = number of tanks

	//filling the map with the necessary categories
	EnemyCount.Add(EEnemyTypes::Melee, 0);
	EnemyCount.Add(EEnemyTypes::Ranged, 0);
	EnemyCount.Add(EEnemyTypes::Tank, 0);

	//creating an array of enemies from the aviable ones, sorted from the lowest price to the highest
	TArray<FInternalDumbEnemyType> SortedEnemies = AviableEnemies;
	SortedEnemies.Sort([](const FInternalDumbEnemyType& A, const FInternalDumbEnemyType& B) {return A.Cost < B.Cost; });

	//the cycle when the magic happens
	while (RemainingPoints > 0)
	{
		//bool that check if a cycle ended with a purchase or not
		bool bAddedEnemy = false;

		for (const FInternalDumbEnemyType& Enemy : SortedEnemies)
		{
			//checking if the pool of points have still budget
			if (Enemy.Cost <= RemainingPoints)
			{
				//checking if there are already enough tanks, skip if true
				if (Enemy.EnemyType == EEnemyTypes::Tank && EnemyCount[EEnemyTypes::Tank] >= MaxTanks)
				{
					continue;
				}
				//create a balance between melee and ranged, trying to pick first one then another
				//TODO: Fix bug spawn enemies	
				if ((Enemy.EnemyType == EEnemyTypes::Melee && EnemyCount[EEnemyTypes::Melee] > EnemyCount[EEnemyTypes::Ranged] + 1) ||
					(Enemy.EnemyType == EEnemyTypes::Ranged && EnemyCount[EEnemyTypes::Ranged] > EnemyCount[EEnemyTypes::Melee] + 1))
				{
					continue;
				}

				//adding the enemy to the array of enemies pulled
				if (SelectedEnemies.Contains(Enemy.EnemyClass))
				{
					SelectedEnemies[Enemy.EnemyClass]++;
				}
				else
				{
					SelectedEnemies.Add(Enemy.EnemyClass, 1);
				}
				EnemyCount[Enemy.EnemyType]++;
				RemainingPoints -= Enemy.Cost;
				bAddedEnemy = true;
				break;
			}
		}
		//if this cycle no enemy was added, budget is depleted
		if (!bAddedEnemy)
		{
			break;
		}
	}
	//return a TMap with all the pulled enemies and in wich quantity
	return SelectedEnemies;

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
	if (!Object) return;
	if (FunctionName.IsNone()) return;

	FScriptDelegate Delegate;
	Delegate.BindUFunction(Object, FunctionName);
	OnEnemySpawn.AddUnique(Delegate);

#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Function '%s' bound to OnEnemySpawn."), *FunctionName.ToString());
#endif

}

void APWaveManager::UnbindOnEnemySpawn(UObject* Object)
{
	if (!Object) return;

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
	OnEnemyDie.AddUnique(Delegate);

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

