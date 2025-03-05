// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "PDataWaveContainer.h"

class WAVETOOL_API SWaveToolWidgetMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWaveToolWidgetMenu){}
		SLATE_ARGUMENT(UPDataWaveContainer*, DataWaveContainer)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	FReply OnTestButtonClicked();
	void OnTestCheckBoxChanged(ECheckBoxState NewState);
	ECheckBoxState IsTestCheckBoxChecked() const;

private:
	bool bIsTestBoxChecked;
	TArray<UPDataWaveContainer*> DataWaveContainersArray;
	TSharedPtr<SVerticalBox> PropertyList;
	void GenerateDataTableWidget();
	FReply OnSetWaveDataButtonClicked();
	FReply SaveWaveDataAsset();
	TArray<FString> GetAllWaveDataAssetPaths();
	TArray<UPDataWaveContainer*> GetDataWaveContainersFromPaths();
	
	// Since StaticCastSharedPtr does not work at runtime we need to add a manual way of checking
	enum class ETreeNodeType
	{
		WaveContainer,
		WavesArray,
		EnemyType
	};
	struct FBaseTreeNode {
		virtual ~FBaseTreeNode(){}
		virtual ETreeNodeType GetNodeType() const = 0;
	};
	struct FEnemyTypesNode : public FBaseTreeNode
	{
		TSubclassOf<AActor>* EnemyClass;
		int32 Cost;
		EEnemyTypes EnemyType;

		FEnemyTypesNode() {};
		FEnemyTypesNode(TSubclassOf<AActor>* enemyClass, int32 cost, EEnemyTypes enemyType) :
			EnemyClass(enemyClass), Cost(cost), EnemyType(enemyType) { }

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

		FWavesArrayNode() {}
		FWavesArrayNode(const int32 waveID, const FString& description, const int32 totalPoints) :
			WaveID(waveID), Description(description), TotalPoints(totalPoints) { }

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
			WaveContainerName(waveContainerName), WaveContainerID(waveContainerID), ContainerDescription(containerDescription){ }

		virtual ETreeNodeType GetNodeType() const override
		{
			return ETreeNodeType::WaveContainer;
		}
	};
	TArray<TSharedPtr<FWaveContainerNode>> WaveContainerNodes;
	TArray<TSharedPtr<FBaseTreeNode>> RootNodes;
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FBaseTreeNode> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void OnGetChildren(TSharedPtr<FBaseTreeNode> InItem, TArray<TSharedPtr<FBaseTreeNode>>& OutChildren);
};
