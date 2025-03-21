// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "PDataWaveContainer.h"

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
	FReply OnMinusButtonPressed(FString assetName);
	bool DeleteDataAsset(FString assetName);
	void RefreshDisplayedDataAssets();
	FReply SaveWaveDataAsset(UPDataWaveContainer* container);
	TArray<FString> GetAllWaveDataAssetPaths();
	TArray<UPDataWaveContainer*> GetDataWaveContainersFromPaths();
	FReply OnWaveContainerSaveButtonClicked(TSharedPtr<FWaveContainerNode> ContainerNode);
	FReply OnWaveContainerPlusButtonClicked();
	FReply OnWavesArrayPlusButtonClicked(int32 containerIndex);
	FReply OnWavesArrayMinusButtonClicked(int32 containerIndex, int32 waveIndex);
	FReply OnEnemyTypesPlusButtonClicked(int32 containerIndex, int32 waveIndex);
	FReply OnEnemyTypesMinusButtonClicked(int32 containerIndex, int32 waveIndex, int32 enemyTypeIndex);


	// Enum drop down menu //
	TArray<TSharedPtr<EEnemyTypes>> EnemyTypes;
	TSharedPtr<EEnemyTypes> SelectedEnemyType;
	void OnEnemyTypeSelectionChanged(TSharedPtr<EEnemyTypes> NewValue, ESelectInfo::Type SelectInfo, int32 containerIndex, int32 waveIndex, int32 enemyTypeIndex);
	TSharedRef<SWidget> GenerateComboItem(TSharedPtr<EEnemyTypes> Item);
	FText GetCurrentItemLabel(int32 containerIndex, int32 waveIndex, int32 enemyTypeIndex) const;
	FText EnumToText(EEnemyTypes EnumValue) const;

	// Spawn Order Drop Down Menu //
	TArray<TSharedPtr<ESpawnOrder>> SpawnOrders;
	TSharedPtr<ESpawnOrder> SelectedSpawnOrder;
	void OnSpawnOrderSelectionChanged(TSharedPtr<ESpawnOrder> NewValue, ESelectInfo::Type SelectInfo, int32 containerIndex, int32 waveIndex);
	TSharedRef<SWidget> GenerateSpawnOrderComboItem(TSharedPtr<ESpawnOrder> Item);
	FText GetCurrentSpawnOrderLabel(int32 containerIndex, int32 waveIndex) const;
	FText EnumToText(ESpawnOrder EnumValue) const;

	// Class Selection Menu //
	void OnClassSelected(const UClass* selectedClass, int32 containerIndex, int32 waveIndex, int32 enemyTypeIndex);
	const UClass* GetSelectedClass(int32 containerIndex, int32 arrayWaveIndex, int32 enemyTypeIndex) const;
	
	// Editables Texts //
	//wave container
	FText GetWaveContainerNameEditableText(const int32 containerIndex) const;
	void OnWaveContainerNameChanged(const FText& NewText, int32 containerIndex);
	FText GetWaveContainerDescriptionEditableText(const int32 containerIndex) const;
	void OnWaveContainerDescriptionChanged(const FText& NewText, int32 containerIndex);
	//wave
	FText GetWaveDescriptionEditableText(const int32 containerIndex, const int32 waveIndex) const;
	void OnWaveDecriptionTextChanged(const FText& NewText, const int32 containerIndex, const int32 waveIndex);
	int32 GetWaveTotalPointsEditableText(const int32 containerIndex, const int32 waveIndex) const;
	void OnWaveTotalPointsTextChanged(const int32 NewValue, const int32 containerIndex, const int32 waveIndex);
	FText GetWaveSpawnOrderEditableText(int32 containerIndex, const int32 waveIndex) const;
	void OnWaveSpawnOrdertextChanged(const ESpawnOrder NewValue, const int32 containerIndex, const int32 waveIndex); 
	//enemy type
	ECheckBoxState GetWaveEnemyUseEnemyCostState(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const;
	void OnWaveEnemyUseEnemyCostStateChanged(const ECheckBoxState NewValue, const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex);
	int32 GetWaveEnemyMinEnemiesEditableText(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const;
	void OnWaveEnemyMinEnemiesTextChanged(const int32 NewValue, const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex);
	int32 GetWaveEnemyMaxEnemiesEditableText(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const;
	void OnWaveEnemyMaxEnemiesTextChanged(const int32 NewValue, const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex);
	int32 GetWaveEnemyCostEditableText(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const;
	void OnWaveEnemyCostTextChanged(const int32 NewValue, const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex);
	FText GetWaveEnemyTypeEditableText(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const;
	void OnWaveEnemyTypeTextChanged(const EEnemyTypes NewValue, const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex);
};
