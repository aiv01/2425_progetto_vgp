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
		TSubclassOf<AActor> EnemyClass;
		int32 Cost;
		EEnemyTypes EnemyType;

		FEnemyTypesNode() {};
		FEnemyTypesNode(TSubclassOf<AActor> enemyClass, int32 cost, EEnemyTypes enemyType) :
			EnemyClass(enemyClass), Cost(cost), EnemyType(enemyType) {
		}

		virtual ETreeNodeType GetNodeType() const override
		{
			return ETreeNodeType::EnemyType;
		}
	};
	struct FWavesArrayNode : public FBaseTreeNode
	{
		int32 WaveID;
		FString Description;
		int32 TotalPoints;
		TArray<TSharedPtr<FEnemyTypesNode>> EnemyTypes;
		TArray<TSharedPtr<FBaseTreeNode>> Children;
		ESpawnOrder SpawnOrder;

		FWavesArrayNode() {}
		FWavesArrayNode(const int32 waveID, const FString& description, const int32 totalPoints) :
			WaveID(waveID), Description(description), TotalPoints(totalPoints) {
		}

		virtual ETreeNodeType GetNodeType() const override
		{
			return ETreeNodeType::WavesArray;
		}
	};
	struct FWaveContainerNode : public FBaseTreeNode
	{
		FString WaveContainerName;
		int32 WaveContainerID;
		FString ContainerDescription;
		TArray<TSharedPtr<FWavesArrayNode>> WavesArray;
		TArray<TSharedPtr<FBaseTreeNode>> Children;

		FWaveContainerNode() {}
		FWaveContainerNode(const FString& waveContainerName, const int32 waveContainerID, const FString& containerDescription) :
			WaveContainerName(waveContainerName), WaveContainerID(waveContainerID), ContainerDescription(containerDescription) {
		}

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
	FText GetCurrentItemLabel() const;
	FText EnumToText(EEnemyTypes EnumValue) const;

	// Class Selection Menu //
	UPROPERTY()	//to avoid garbage collector
	TSubclassOf<AActor> SelectedActorClass = nullptr;
	void OnClassSelected(const UClass* selectedClass, int32 containerIndex, int32 waveIndex, int32 enemyTypeIndex);
	const UClass* GetSelectedClass() const;
	
	// Editables Texts //
	//wave container
	int32 GetWaveContainerIDEditableText(const int32 containerIndex) const;
	void OnWaveContainerIDChanged(const int32 NewValue, const int32 containerIndex);
	FText GetWaveContainerDescriptionEditableText(const int32 containerIndex) const;
	void OnWaveContainerDescriptionChanged(const FText& NewText, int32 containerIndex);
	//wave
	int32 GetWaveIDEditableText(const int32 containerIndex, const int32 waveIndex) const;
	void OnWaveIDValueChange(const int32 NewValue, const int32 containerIndex, const int32 waveIndex);
	FText GetWaveDescriptionEditableText(const int32 containerIndex, const int32 waveIndex) const;
	void OnWaveDecriptionTextChanged(const FText& NewText, const int32 containerIndex, const int32 waveIndex);
	int32 GetWaveTotalPointsEditableText(const int32 containerIndex, const int32 waveIndex) const;
	void OnWaveTotalPointsTextChanged(const int32 NewValue, const int32 containerIndex, const int32 waveIndex);
	FText GetWaveSpawnOrderEditableText(int32 containerIndex, const int32 waveIndex) const;
	void OnWaveSpawnOrdertextChanged(const ESpawnOrder NewValue, const int32 containerIndex, const int32 waveIndex); 
	//enemy type
	int32 GetWaveEnemyCostEditableText(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const;
	void OnWaveEnemyCostTextChanged(const int32 NewValue, const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex);
	FText GetWaveEnemyTypeEditableText(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const;
	void OnWaveEnemyTypeTextChanged(const EEnemyTypes NewValue, const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex);
};
