// Fill out your copyright notice in the Description page of Project Settings.


#include "PWaveManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "PDataWaveContainer.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnInfoInterface.h"
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
	Instance = this;
	CacheSpawnerWeights(); 
	if (!WaveDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: Data Asset not assigned."));
		return;
	}

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

void APWaveManager::SpawnNextEnemy()
{
    if (PendingSpawnQueue.Num() == 0)
    {
        CheckWaveCompletion();
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    FRotator SpawnRotation = GetActorRotation();

    float NextDelay = SpawnFrequency;

    if (bUseFormation && PendingSpawnQueue[0].GroupID != -1)
    {
        
        int32 CurrentGroupID = PendingSpawnQueue[0].GroupID;
        TArray<FSpawnInstruction> GroupInstructions;

        while (PendingSpawnQueue.Num() > 0 && PendingSpawnQueue[0].GroupID == CurrentGroupID)
        {
            GroupInstructions.Add(PendingSpawnQueue[0]);
            PendingSpawnQueue.RemoveAt(0);
        }

        AWaveSpawner* ChosenSpawner = GetRandomSpawnerByWeight();
        FVector BaseLocation = ChosenSpawner ? ChosenSpawner->GetRandomSpawnLocation() : GetActorLocation();

        int32 GroupSize = GroupInstructions.Num();
        float Radius = 100.0f;
        float AngleStep = 360.f / GroupSize;

        for (int32 i = 0; i < GroupSize; i++)
        {
            float AngleRadians = FMath::DegreesToRadians(AngleStep * i);
            FVector Offset = FVector(FMath::Cos(AngleRadians), FMath::Sin(AngleRadians), 0.f) * Radius;

            FVector SpawnLocation = GroupInstructions[i].SpawnPosition != FVector::ZeroVector
                                      ? GroupInstructions[i].SpawnPosition
                                      : BaseLocation + Offset;

            AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(GroupInstructions[i].EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
			
            if (SpawnedEnemy)
            {
            	if (SpawnedEnemy->GetClass()->ImplementsInterface(UEnemySpawnInfoInterface::StaticClass()))
            	{
            		IEnemySpawnInfoInterface::Execute_SetSpawnInfo(SpawnedEnemy, GroupInstructions[i].bIsLeader, GroupInstructions[i].FormationName, GroupInstructions[i].GroupID);
            	}else
            	{
            		UE_LOG(LogTemp, Error, TEXT("Wave Manager: Enemy doesn't implement interface!"));
            	}
                OnEnemySpawn.Broadcast(SpawnedEnemy,GroupInstructions[i].bIsLeader, GroupInstructions[i].FormationName, GroupInstructions[i].GroupID);
            }
        }

        NextDelay = FormationSpawnFrequency;
    }
    else
    {
        // Single Spawn without team
        FSpawnInstruction Instruction = PendingSpawnQueue[0];
        PendingSpawnQueue.RemoveAt(0);

        FVector SpawnLocation = PrecomputedSpawnPositions.Num() > 0
                                    ? PrecomputedSpawnPositions[0]
                                    : GetActorLocation();

        if (PrecomputedSpawnPositions.Num() > 0)
            PrecomputedSpawnPositions.RemoveAt(0);

        AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(Instruction.EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
        
        if (SpawnedEnemy)
        {
        	
            OnEnemySpawn.Broadcast(SpawnedEnemy, false, "None", -1);
#if UE_BUILD_DEVELOPMENT
        	if(bEnableLogging)
        	{
            UE_LOG(LogTemp, Log, TEXT("WaveManager: Spawned enemy '%s'."), *SpawnedEnemy->GetName());
        	}
#endif
        }
    }

    GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this, &APWaveManager::SpawnNextEnemy, NextDelay, false);
}

void APWaveManager::GenerateSpawnQueue()
{
	PendingSpawnQueue.Empty();

	const int32 WavePoints = WavesArray[CurrentWaveIndex].WaveSettings.TotalPoints;
	const auto& EnemyList = GenerateWave(WavePoints, 3, WavesArray[CurrentWaveIndex].WaveSettings.EnemyTypes);

	for (const auto& Pair : EnemyList)
	{
		for (int32 i = 0; i < Pair.Value; ++i)
		{
			PendingSpawnQueue.Add(FSpawnInstruction{Pair.Key});
		}
	}

	if (bUseFormation)
	{
		ApplyFormationsToSpawnQueue_RoundRobin();
		PrecomputedSpawnPositions = ComputeSpawnPositionsFromComposition(ComputeSpawnQueueComposition());

		if (bEnableLogging)
		{
			TSet<FName> FormationsGenerated;
			for (const auto& Instruction : PendingSpawnQueue)
			{
				if (Instruction.FormationName != NAME_None)
					FormationsGenerated.Add(Instruction.FormationName);
			}

			UE_LOG(LogTemp, Log, TEXT("Generated %d formations:"), FormationsGenerated.Num());
			for (const FName& Formation : FormationsGenerated)
			{
				UE_LOG(LogTemp, Log, TEXT("- %s"), *Formation.ToString());
			}

			for (int32 i = 0; i < PendingSpawnQueue.Num(); i++)
			{
				UE_LOG(LogTemp, Log, TEXT("Enemy %s will spawn at spawner %s."),
					*PendingSpawnQueue[i].EnemyClass->GetName(),
					PrecomputedSpawnerNames.IsValidIndex(i) ? *PrecomputedSpawnerNames[i] : TEXT("Unknown"));
			}
		}
	}
	else
	{
		PendingSpawnQueue.Sort([](const FSpawnInstruction&, const FSpawnInstruction&)
		{
			return FMath::FRand() < 0.5f;
		});

		PrecomputedSpawnPositions = ComputeSpawnPositions(PendingSpawnQueue.Num());

		if (bEnableLogging)
		{
			UE_LOG(LogTemp, Log, TEXT("Generated %d enemies without formations."), PendingSpawnQueue.Num());
			for (int32 i = 0; i < PendingSpawnQueue.Num(); i++)
			{
				UE_LOG(LogTemp, Log, TEXT("Enemy %s will spawn at spawner %s."),
				*PendingSpawnQueue[i].EnemyClass->GetName(),
				PrecomputedSpawnerNames.IsValidIndex(i) ? *PrecomputedSpawnerNames[i] : TEXT("Unknown"));
			}
		}
	}

	ActiveEnemies = PendingSpawnQueue.Num();
}


AWaveSpawner* APWaveManager::GetRandomSpawnerByWeight() const
{
	float RandomWeight = FMath::FRandRange(0.f, CachedTotalSpawnerWeight);
	for (AWaveSpawner* Spawner : Spawners)
	{
		RandomWeight -= Spawner->SpawnWeight;
		if (RandomWeight <= 0.f) return Spawner;
	}
	return Spawners.Num() > 0 ? Spawners[0] : nullptr;
}
void APWaveManager::CacheSpawnerWeights()
{
	CachedTotalSpawnerWeight = 0.f;
	for (const AWaveSpawner* Spawner : Spawners)
	{
		CachedTotalSpawnerWeight += Spawner ? Spawner->SpawnWeight : 0.f;
	}
}
TArray<FVector> APWaveManager::ComputeSpawnPositions(int32 TotalEnemies) 
{
	TArray<FVector> Positions;
	for (int32 i = 0; i < TotalEnemies; ++i)
	{
		if (AWaveSpawner* Spawner = GetRandomSpawnerByWeight())
		{
			Positions.Add(Spawner->GetRandomSpawnLocation());
			PrecomputedSpawnerNames.Add(Spawner->GetName());
		}		else
		{
			Positions.Add(GetActorLocation());
			PrecomputedSpawnerNames.Add(TEXT("DefaultSpawner"));
		}
	}
	return Positions;
}


TArray<FVector> APWaveManager::ComputeSpawnPositionsFromComposition(const TArray<int32>& Composition)
{
	PrecomputedSpawnerNames.Empty();
	TArray<FVector> SpawnPositions;

	for (int32 GroupCount : Composition)
	{
		AWaveSpawner* ChosenSpawner = GetRandomSpawnerByWeight();
		FString ChosenSpawnerName = ChosenSpawner ? ChosenSpawner->GetName() : TEXT("Unknown");

		for (int32 i = 0; i < GroupCount; i++)
		{
			FVector SpawnLocation = ChosenSpawner ? ChosenSpawner->GetRandomSpawnLocation() : GetActorLocation();

			SpawnPositions.Add(SpawnLocation);
			PrecomputedSpawnerNames.Add(ChosenSpawnerName); // Same spawner for all enemy in the group
		}
	}

	return SpawnPositions;
}



void APWaveManager::ApplyFormationsToSpawnQueue_RoundRobin()
{
    if (FormationDataAssets.Num() == 0)
    {
        return;
    }

    TArray<FSpawnInstruction> TempQueue = PendingSpawnQueue;
    PendingSpawnQueue.Empty();
	
    TMap<FName, TArray<TArray<FSpawnInstruction>>> GroupsByFormation;

	int32 GroupCounter = 0;

    bool bAnyGroupFormed = true;
    while (bAnyGroupFormed)
    {
        bAnyGroupFormed = false;
        for (UFormationDataAsset* FormationAsset : FormationDataAssets)
        {
            if (!FormationAsset)
            {
                continue;
            }

            bool bCanFormGroup = true;
            for (const FEnemyFormationEntry& Entry : FormationAsset->FormationEntries)
            {
                int32 CountFound = 0;
                for (const FSpawnInstruction& Inst : TempQueue)
                {
                    if (Inst.EnemyClass == Entry.EnemyClass && Inst.FormationName == NAME_None)
                    {
                        CountFound++;
                    }
                }
                if (CountFound < Entry.Quantity)
                {
                    bCanFormGroup = false;
                    break;
                }
            }
        	
            if (bCanFormGroup)
            {
                TArray<FSpawnInstruction> Group;
                for (const FEnemyFormationEntry& Entry : FormationAsset->FormationEntries)
                {
                    int32 Assigned = 0;
                    TArray<int32> IndicesToRemove;
                    for (int32 i = 0; i < TempQueue.Num() && Assigned < Entry.Quantity; i++)
                    {
                        if (TempQueue[i].EnemyClass == Entry.EnemyClass && TempQueue[i].FormationName == NAME_None)
                        {
                            FSpawnInstruction Inst = TempQueue[i];
                            Inst.FormationName = FormationAsset->FormationName;
                        	Inst.GroupID = GroupCounter;
                            Inst.bIsLeader = (Entry.bIsLeader && Assigned == 0);
                            Group.Add(Inst);
                            IndicesToRemove.Add(i);
                            Assigned++;
                            UE_LOG(LogTemp, Log, TEXT("WaveManager: Assign enemy %s, with formation %s, is leader %s."),
                                   *Inst.EnemyClass->GetName(),
                                   *Inst.FormationName.ToString(),
                                   Inst.bIsLeader ? TEXT("true") : TEXT("false"));
                        }
                    }
                    IndicesToRemove.Sort([](int32 A, int32 B){ return A > B; });
                    for (int32 Idx : IndicesToRemove)
                    {
                        TempQueue.RemoveAt(Idx);
                    }
                }
                GroupsByFormation.FindOrAdd(FormationAsset->FormationName).Add(Group);
            	GroupCounter++;
                bAnyGroupFormed = true;
            }
        }
    }
	
    TArray<FSpawnInstruction> Unformed = TempQueue;

    TArray<FSpawnInstruction> FormedFlat;

    for (auto& Pair : GroupsByFormation)
    {
        for (const TArray<FSpawnInstruction>& Group : Pair.Value)
        {
            FormedFlat.Append(Group);
        }
    }

	TArray<FName> FormationOrder;
	for (UFormationDataAsset* Asset : FormationDataAssets)
	{
		if (Asset && !FormationOrder.Contains(Asset->FormationName))
		{
			FormationOrder.Add(Asset->FormationName);
		}
	}
	TArray<FSpawnInstruction> InterlacedQueue = GroupFormationInstructions_RoundRobin(GroupsByFormation, FormationOrder);
	InterlacedQueue.Append(Unformed);
	PendingSpawnQueue = InterlacedQueue;
}

TArray<FSpawnInstruction> APWaveManager::GroupFormationInstructions_RoundRobin(
	const TMap<FName, TArray<TArray<FSpawnInstruction>>>& GroupsByFormation, const TArray<FName>& FormationOrder)
{
	TArray<FSpawnInstruction> FinalQueue;

	int32 MaxGroupCount = 0;
	for (const FName& FormName : FormationOrder)
	{
		if (GroupsByFormation.Contains(FormName))
		{
			MaxGroupCount = FMath::Max(MaxGroupCount, GroupsByFormation[FormName].Num());
		}
	}

	for (int32 GroupIdx = 0; GroupIdx < MaxGroupCount; GroupIdx++)
	{
		for (const FName& FormName : FormationOrder)
		{
			if (GroupsByFormation.Contains(FormName))
			{
				const TArray<TArray<FSpawnInstruction>>& Groups = GroupsByFormation[FormName];
				if (Groups.IsValidIndex(GroupIdx))
				{
					FinalQueue.Append(Groups[GroupIdx]);
				}
			}
		}
	}
	return FinalQueue;
}

TArray<int32> APWaveManager::ComputeSpawnQueueComposition() const
{
	TArray<int32> Composition;
	int32 CurrentGroup = -1, Count = 0;

	for (const auto& Inst : PendingSpawnQueue)
	{
		if (Inst.GroupID != CurrentGroup)
		{
			if (Count > 0) Composition.Add(Count);
			CurrentGroup = Inst.GroupID;
			Count = 1;
		}
		else
		{
			++Count;
		}
	}
	if (Count > 0) Composition.Add(Count);
	return Composition;
}



#pragma endregion

#pragma region Greedy
TMap<TSubclassOf<AActor>, int32> APWaveManager::GenerateWave(const int32 WavePoints, const int32 PlayerCount,
                                                             const TArray<FInternalDumbEnemyType>& AviableEnemies)
{
	// Variables
	TMap<TSubclassOf<AActor>, int32> SelectedEnemies;			// return map, contains selected enemies
	TMap<EEnemyTypes, int32> EnemyCount;						// track how many enemy types are selected 
	TMap<EEnemyTypes, int32> PointBuyCount;						// Enemy count used only by "Point buy"
	int32 RemainingPoints = WavePoints;							// quantity of remaining points
	int32 MaxTanks = PlayerCount;								// Tank limited by player count (Point Buy Only)

	// Initializing EnemyCount
	EnemyCount.Add(EEnemyTypes::Melee, 0);
	EnemyCount.Add(EEnemyTypes::Ranged, 0);
	EnemyCount.Add(EEnemyTypes::Tank, 0);

	// Initializing PointBuyCount
	PointBuyCount.Add(EEnemyTypes::Melee, 0);
	PointBuyCount.Add(EEnemyTypes::Ranged, 0);
	PointBuyCount.Add(EEnemyTypes::Tank, 0);

	// Order the enemies by their cost, from the most to the least expensive 
	TArray<FInternalDumbEnemyType> SortedEnemies = AviableEnemies;
	SortedEnemies.Sort([](const FInternalDumbEnemyType& A, const FInternalDumbEnemyType& B)
	{
		return A.Cost > B.Cost;
	});


	auto AddEnemy = [&](const FInternalDumbEnemyType& Enemy, int32 Count, bool bIsPointBuy)
	{
		if (Count <= 0) return; //Prevent more enemies that the quantity

		SelectedEnemies.FindOrAdd(Enemy.EnemyClass) += Count;
		EnemyCount[Enemy.EnemyType] += Count;
		if (bIsPointBuy) // if the enemy is taken by point buy, we update his map
		{
			PointBuyCount[Enemy.EnemyType] += Count;
		}
	};

	//Min-Max
	for (const FInternalDumbEnemyType& Enemy : SortedEnemies)
	{
		if (!Enemy.bUseEnemyCost) 
		{
			int32 SpawnCount = InternalRandom(Enemy.MinEnemies, Enemy.MaxEnemies);

			AddEnemy(Enemy, SpawnCount, false);
		}
	}

	//Point Buy
	while (RemainingPoints > 0)
	{
		bool bAddedEnemy = false;

		for (const FInternalDumbEnemyType& Enemy : SortedEnemies)
		{
			if (!Enemy.bUseEnemyCost || Enemy.Cost > RemainingPoints)
			{
				continue; // Skip min-max enemies and check if purchases is still possible
			}

			// check the number of tanks
			if (Enemy.EnemyType == EEnemyTypes::Tank && EnemyCount[EEnemyTypes::Tank] >= MaxTanks)
			{
				continue;
			}

			bool bOnlyMelee = (PointBuyCount[EEnemyTypes::Ranged] == 0) && Enemy.bUseEnemyCost;
			bool bOnlyRanged = (PointBuyCount[EEnemyTypes::Melee] == 0) && Enemy.bUseEnemyCost;

			if (!bOnlyMelee && !bOnlyRanged)
			{
				if ((Enemy.EnemyType == EEnemyTypes::Melee && PointBuyCount[EEnemyTypes::Melee] > PointBuyCount[
						EEnemyTypes::Ranged] + 1) ||
					(Enemy.EnemyType == EEnemyTypes::Ranged && PointBuyCount[EEnemyTypes::Ranged] > PointBuyCount[
						EEnemyTypes::Melee] + 1))
				{
					continue;
				}
			}
			// Adding the enemy
			AddEnemy(Enemy, 1, true);
			RemainingPoints -= Enemy.Cost;
			bAddedEnemy = true;
		}

		//If no enemy is added, stop the loop
		if (!bAddedEnemy)
		{
			break;
		}
	}

	//Return of the map with Min-Max and Point Buy Enemies
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
		EndLevel();
		return;
	}
	
	GenerateSpawnQueue();
	SpawnNextEnemy();
	OnWaveStarted.Broadcast(CurrentWaveIndex + 1);
}

void APWaveManager::CheckWaveCompletion()
{
	ActiveEnemies = FMath::Max(ActiveEnemies - 1, 0);
	if (ActiveEnemies == 0)
	{
		OnWaveCompleted.Broadcast(++CurrentWaveIndex);
		GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this, &APWaveManager::StartWave, WaveInterval, false);
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
			GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this, &APWaveManager::StartWave, WaveInterval,
			                                       false);
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
                                          const TArray<AWaveSpawner*>& NewSpawners, const float NewWaveInterval,
                                          const bool bNewAutoStartWaveSystem)
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
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Initialized with new data. WaveInterval: %f, Spawners: %d"), WaveInterval,
	       Spawners.Num());
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
	if (!WaveDataAsset || WaveDataAsset->WavesArray.Num() == 0) return;

	WavesArray = WaveDataAsset->WavesArray;
	WavesArray.Sort([](const FWaveOrder& A, const FWaveOrder& B)
	{
		return A.SpawnOrder < B.SpawnOrder;
	});

	CurrentWaveIndex = 0;
	GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, this, &APWaveManager::StartWave, 1.f, false);
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
