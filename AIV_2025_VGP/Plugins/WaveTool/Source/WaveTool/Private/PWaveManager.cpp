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


APWaveManager::APWaveManager() : WaveDataAsset(nullptr), WaveInterval(0), bAutoStartWaveSystem(false), CurrentWaveIndex(0),
ActiveEnemies(0)
{
	//Disable tick 
	PrimaryActorTick.bCanEverTick = false;
}

/*AWaveManager::AWaveManager(UDumbWavesContainerAsset* NewWaveData, const TArray<FTransform>& SpawnerLocations,
						   const float WaveInterval, const bool AutoStartWaveSystem) : WaveDataAsset(NewWaveData), WaveInterval(WaveInterval), bAutoStartWaveSystem(AutoStartWaveSystem), EnemySpawners(SpawnerLocations), CurrentWaveIndex(0), ActiveEnemies(0)
{
	PrimaryActorTick.bCanEverTick = false;
}*/

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

/*AWaveManager* AWaveManager::CreateWaveManagerInLevel(UDumbWavesContainerAsset* NewWaveData, bool bAutoStart, float WavesInterval)
{
#if !WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("WaveManager: This function can only be called in Editor Mode!"));
	return nullptr;
#endif

	// Get world istance using "Editor.h"
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: Unable to access World in Editor!"));
		return nullptr;
	}

	//Check if there's already one manager in the level
	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(World, AWaveManager::StaticClass(), FoundManagers);

	if (FoundManagers.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: An instance already exists in the level. Updating existing one."));
		AWaveManager* ExistingManager = Cast<AWaveManager>(FoundManagers[0]);
		if (ExistingManager)
		{
			ExistingManager->SetWaveData(NewWaveData, true);
			ExistingManager->bAutoStartWaveSystem = bAutoStart;
		}
		return ExistingManager;
	}

	//Create new manager
	 //*SpawnParameters is a struct used for assing param to the actor to spawn

	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = TEXT("WaveManager");
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWaveManager* NewManager = World->SpawnActor<AWaveManager>(AWaveManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (!NewManager)
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: Failed to spawn instance!"));
		return nullptr;
	}
	//Set WaveMngrParams
	NewManager->SetWaveData(NewWaveData, false);
	NewManager->bAutoStartWaveSystem = bAutoStart;
	NewManager->SetWaveInterval(WavesInterval);

	//TODO: Function for assign manager param

#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Created successfully in the level."));
#endif
	return NewManager;
}
*/

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
	StartWave();
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

//DEPRECATED

//void AWaveManager::SpawnWave()
//{
//	if (CurrentWaveIndex >= WavesArray.Num())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("WaveManager: No valid wave at index %d."), CurrentWaveIndex);
//		return;
//	}
//	//Get current Wave
//	const FWaveOrder& CurrentWaveOrder = WavesArray[CurrentWaveIndex];
//#if UE_BUILD_DEVELOPMENT
//	UE_LOG(LogTemp, Log, TEXT("WaveManager: Spawning enemies for wave %d."), CurrentWaveIndex + 1);
//	UE_LOG(LogTemp, Log, TEXT("WaveManager: Enemytypes -> %d."), CurrentWaveOrder.WaveSettings.EnemyTypes.Num());
//#endif
//	//Reset counter
//	ActiveEnemies = 0;
//
//	//For every setting in currentWaveData && for every enemy define in setting, spawn 5 enemy
//	//for (const FDumbWave& WaveSetting : CurrentWaveOrder.WaveSettings)
//	//{
//	for (const FInternalDumbEnemyType& EnemyType : CurrentWaveOrder.WaveSettings.EnemyTypes)
//	{
//		if (!EnemyType.EnemyClass)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("WaveManager: Invalid enemy class in wave %d."), CurrentWaveOrder.WaveSettings.WaveID);
//			continue;
//		}
//		const int32 SpawnCount = 5; //TODO: Create the algoritm for spawning enemies using the cost
//		for (int32 i = 0; i < SpawnCount; ++i)
//		{
//			UE_LOG(LogTemp, Log, TEXT("SpawnCount for loop %d"), SpawnCount);
//			FVector SpawnLocation = GetActorLocation();
//			FRotator SpawnRotation = GetActorRotation();
//			if (EnemySpawners.Num() > 0)
//			{
//				int32 RandomIndex = FMath::RandRange(0, EnemySpawners.Num() - 1);
//				SpawnLocation = EnemySpawners[RandomIndex].GetLocation();
//				SpawnRotation = EnemySpawners[RandomIndex].Rotator();
//			}
//			FActorSpawnParameters SpawnParams;
//			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
//			AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(EnemyType.EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
//			if (SpawnedEnemy)
//			{
//#if UE_BUILD_DEVELOPMENT
//				UE_LOG(LogTemp, Log, TEXT("WaveManager: Spawned enemy '%s' of type '%s'."), *SpawnedEnemy->GetName(), *EnemyType.EnemyClass->GetName());
//#endif
//				ActiveEnemies++;
//				//TODO: Delegate for HandleEnemyDie()!
//			}
//			else
//			{
//				UE_LOG(LogTemp, Error, TEXT("WaveManager: Failed to spawn enemy of type '%s'."), *EnemyType.EnemyClass->GetName());
//			}
//		}
//	}
//	//}
//#if UE_BUILD_DEVELOPMENT
//	UE_LOG(LogTemp, Log, TEXT("WaveManager: Active enemies count set to %d."), ActiveEnemies);
//#endif 
//}

//--------------------------------------------------------------------------------

void APWaveManager::NewStartWave()
{
	if (CurrentWaveIndex >= WavesArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: No valid wave at index %d."), CurrentWaveIndex);
		return;
	}
	//Get current Wave
	const FWaveOrder& CurrentWaveOrder = WavesArray[CurrentWaveIndex];
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Spawning enemies for wave %d."), CurrentWaveIndex + 1);
#endif
	//Reset counter
	ActiveEnemies = 0;
	TMap<TSubclassOf<AActor>, int32> EnemyList;
	int poolOfPoint = WaveDataAsset->WavesArray[0].WaveSettings.TotalPoints;
	TArray<FInternalDumbEnemyType> Enemies = WaveDataAsset->WavesArray[0].WaveSettings.EnemyTypes;
	EnemyList = GenerateWave(poolOfPoint, 3, Enemies);
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: EnemyList Num -> %d"), EnemyList.Num());
#endif

	for (const TPair<TSubclassOf<AActor>, int32>& Pair : EnemyList)
	{
		TSubclassOf<AActor> EnemyClass = Pair.Key;
		int32 SpawnCount = Pair.Value;

		for (int32 i = 0; i < SpawnCount; ++i)
		{
			FVector SpawnLocation = GetActorLocation();
			FRotator SpawnRotation = GetActorRotation();

			if (EnemySpawners.Num() > 0)
			{
				int32 RandomIndex = FMath::RandRange(0, EnemySpawners.Num() - 1);
				SpawnLocation = EnemySpawners[RandomIndex].GetLocation();
				SpawnRotation = EnemySpawners[RandomIndex].Rotator();
			}

			SpawnLocation += FVector((double)FMath::RandRange(-300, 300), (double)FMath::RandRange(-300, 300), 0.0);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (SpawnedEnemy)
			{
				UE_LOG(LogTemp, Log, TEXT("WaveManager: Spawned enemy '%s' of type '%s'."), *SpawnedEnemy->GetName(), *EnemyClass->GetName());
				ActiveEnemies++;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("WaveManager: Failed to spawn enemy of type '%s'."), *EnemyClass->GetName());
			}
		}
	}
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Active enemies count set to %d."), ActiveEnemies);
#endif 

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
	// TODO: Implement Delegate
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Ending level."));
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
	const TArray<FTransform>& SpawnerLocations, const float NewWaveInterval, const bool bNewAutoStartWaveSystem)
{
	if (!NewWaveData)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: Invalid Data Asset provided."));
		return;
	}

	WaveDataAsset = NewWaveData;
	EnemySpawners = SpawnerLocations;
	this->WaveInterval = WaveInterval;
	this->bAutoStartWaveSystem = bNewAutoStartWaveSystem;
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Initialized with new data. WaveInterval: %f, Spawners: %d"), WaveInterval, EnemySpawners.Num());
#endif
}

