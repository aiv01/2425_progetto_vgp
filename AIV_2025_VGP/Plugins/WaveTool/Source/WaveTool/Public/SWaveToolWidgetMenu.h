// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "PDataWaveContainer.h"
#include "IDetailsView.h"
#include "PWaveManager.h"
#include "SwaveToolWidgetMenu.generated.h"

enum EWidgetTab {
	WaveContainers,
	WaveManagers
};

UCLASS()
class UFormationAssetListObject : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Formations")
		TArray<UFormationDataAsset*> FormationAssets;
};

class WAVETOOL_API SWaveToolWidgetMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWaveToolWidgetMenu){}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	enum class ETreeNodeType
	{
		WaveContainer,
		WavesArray,
		EnemyType
	};
	struct FBaseTreeNode {
		virtual ~FBaseTreeNode() {}
		virtual ETreeNodeType GetNodeType() const = 0;
	};
	struct FEnemyTypesNode : public FBaseTreeNode
	{
		int32 EnemyTypesID;
		int32 WaveID;
		int32 WaveContainerID;
		TSubclassOf<AActor> EnemyClass;
		bool UseEnemyCost;
		int32 MinEnemies;
		int32 MaxEnemies;
		int32 Cost;
		EEnemyTypes EnemyType;

		FEnemyTypesNode() {};

		virtual ETreeNodeType GetNodeType() const override
		{
			return ETreeNodeType::EnemyType;
		}
	};
	struct FWavesArrayNode : public FBaseTreeNode
	{
		int32 WaveID;
		int32 WaveContainerID;
		FString Description;
		int32 TotalPoints;
		TArray<TSharedPtr<FEnemyTypesNode>> EnemyTypes;
		TArray<TSharedPtr<FBaseTreeNode>> Children;
		ESpawnOrder SpawnOrder;

		FWavesArrayNode() {}

		virtual ETreeNodeType GetNodeType() const override
		{
			return ETreeNodeType::WavesArray;
		}
	};
	struct FWaveContainerNode : public FBaseTreeNode
	{
		int32 WaveContainerID;
		FString WaveContainerName;
		FString ContainerDescription;
		TArray<TSharedPtr<FWavesArrayNode>> WavesArray;
		TArray<TSharedPtr<FBaseTreeNode>> Children;

		FWaveContainerNode() {}

		virtual ETreeNodeType GetNodeType() const override
		{
			return ETreeNodeType::WaveContainer;
		}
	};
	TArray<TSharedPtr<FWaveContainerNode>> WaveContainerNodes;
	TArray<TSharedPtr<FBaseTreeNode>> RootNodes;
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FBaseTreeNode> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void OnGetChildren(TSharedPtr<FBaseTreeNode> InItem, TArray<TSharedPtr<FBaseTreeNode>>& OutChildren);


	bool bIsTestBoxChecked;
	const FString WaveContainerPath = TEXT("/Game/Custom/WaveSystem/WaveContainers/");
	TArray<UPDataWaveContainer*> DataWaveContainersArray;
	TSharedPtr<SVerticalBox> PropertyList;
	void GenerateDataTableWidget();
	FReply OnSetWaveDataButtonClicked();
	bool DeleteDataAsset(FString assetName);
	void RefreshDisplayedDataAssets();
	FReply SaveWaveDataAsset(UPDataWaveContainer* container);
	TArray<FString> GetAllWaveDataAssetPaths();
	TArray<UPDataWaveContainer*> GetDataWaveContainersFromPaths();
	FReply OnWaveContainerSaveButtonClicked(TSharedPtr<FWaveContainerNode> ContainerNode);

	EWidgetTab WidgetTab;

	//Second Tab//
	APWaveManager* WaveManagerInstance;
	UClass* WManagerDataWaveContainer;
	void FindWaveManager();
	void SpawnBPInstance(FString BPPath);
	struct WaveManagerData
	{
		UPDataWaveContainer* SelectedWaveContainerAsset;
		float WaveInterval;
		float FormationSpawnFrequency;
		float SpawnFrequency;
		ECheckBoxState AutoStartWaveSystem;

		TArray<TSharedPtr<FString>> SpawnerNames;
		TArray<TSharedPtr<FString>> CurrentSpawnerSelections;
		TSharedPtr<SVerticalBox> SpawnerListContainer;
		TArray<AWaveSpawner*> SelectedSpawners;
		TMap<FString, AWaveSpawner*> NameToSpawnerMap;

		TArray<UFormationDataAsset*> SelectedFormations;
		TSharedPtr<SVerticalBox> FormationListContainer;

		ECheckBoxState UseFormation;

		WaveManagerData() :
			SelectedWaveContainerAsset(nullptr),
			WaveInterval(0.0f),
			FormationSpawnFrequency(0.0f),
			SpawnFrequency(0.0f),
			AutoStartWaveSystem(ECheckBoxState::Unchecked),
			SpawnerNames(TArray<TSharedPtr<FString>>()),
			UseFormation(ECheckBoxState::Unchecked)
		{ }
	};
	WaveManagerData WaveManagerData_Instance;
	void RefreshSpawnerList();
	TSharedRef<SWidget> GenerateSpawnerWidget(int32 Index);
	void RefreshFormationList();
	TSharedRef<SWidget> GenerateFormationWidget(int32 Index);
	bool SaveWaveManagerAsset() const;

	// Enum drop down menu //
	TArray<TSharedPtr<EEnemyTypes>> EnemyTypes;
	TSharedPtr<EEnemyTypes> SelectedEnemyType;
	FText EnumToText(EEnemyTypes EnumValue) const;

	// Spawn Order Drop Down Menu //
	TArray<TSharedPtr<ESpawnOrder>> SpawnOrders;
	TSharedPtr<ESpawnOrder> SelectedSpawnOrder;
	FText EnumToText(ESpawnOrder EnumValue) const;

	// Class Selection Menu //
	void OnEnemyClassSelected(const UClass* selectedClass, int32 containerIndex, int32 waveIndex, int32 enemyTypeIndex);
	
	ECheckBoxState GetWaveEnemyUseEnemyCostState(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const;
};