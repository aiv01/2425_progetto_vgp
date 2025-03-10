// Fill out your copyright notice in the Description page of Project Settings.

#include "SWaveToolWidgetMenu.h"
#include "SlateOptMacros.h"
#include "PWaveManager.h"
#include <FileHelpers.h>
#include "EditorAssetLibrary.h"
#include <AssetRegistry/AssetRegistryModule.h>
#include "Widgets/Input/SSpinBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SWaveToolWidgetMenu::Construct(const FArguments& InArgs)
{

	//The issue is that your widget isn't assigning its UI tree to its root slot. 
	// In Slate (when subclassing SCompoundWidget), you must wrap your UI in a ChildSlot
	ChildSlot
	[
		SNew(SHorizontalBox) + SHorizontalBox::Slot()
		[
			SNew(SBorder)
            .BorderBackgroundColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f))
			[
				SNew(SVerticalBox) + SVerticalBox::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(SButton)
						.Text(FText::FromString(TEXT("+")))
						.OnClicked(this, &SWaveToolWidgetMenu::OnPlusButtonClicked)
				]
					+ SVerticalBox::Slot()
				[
					SAssignNew(PropertyList, SVerticalBox)	//don't even think i need this anymore
				]
			]	
		] + SHorizontalBox::Slot()
		[
			SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(1.0f, 0.0f, 0.5f, 1.0f))
                .HAlign(EHorizontalAlignment::HAlign_Center)
                .VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight()
				[
					SNew(SButton)
                        .HAlign(HAlign_Center)
						.Text(FText::FromString(TEXT("SetWaveData")))
						.OnClicked(this, &SWaveToolWidgetMenu::OnSetWaveDataButtonClicked)
				] 
				/*	+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SButton)
                        .HAlign(HAlign_Center)
						.Text(FText::FromString("Save WaveDataAsset"))
						.OnClicked(this, &SWaveToolWidgetMenu::SaveWaveDataAsset, FString(TEXT("SaveWaveDataAsset")))
				]*/
			]
		]
	];
	GenerateDataTableWidget();
	// ENUM DROP DOWN MENU STUFF //
	/*Options.Add(MakeShared<EEnemyTypes>(EEnemyTypes::Melee));
	Options.Add(MakeShared<EEnemyTypes>(EEnemyTypes::Ranged));
	Options.Add(MakeShared<EEnemyTypes>(EEnemyTypes::Tank));
	SelectedOption = Options[0];*/
}

// Show properties, detached from the construct function
void SWaveToolWidgetMenu::GenerateDataTableWidget()
{
	DataWaveContainersArray = GetDataWaveContainersFromPaths();
	if (!PropertyList.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("PropertyList is not valid"));
		return;
	}

	if (DataWaveContainersArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No DataWaveContainers found"));
		PropertyList->AddSlot()
		[
				SNew(STextBlock).Text(FText::FromString(TEXT("No DataWaveContainers found")))
		];
		return;
	}

	// Transfer data from the array of AssetDataWaveContainers to my custom struct
	TArray<TArray<TSharedPtr<FWaveContainerNode>>> RootNodesContainer;
	for (UPDataWaveContainer* DataWaveContainer : DataWaveContainersArray)
	{
		TSharedPtr<FWaveContainerNode> waveContainerNode = MakeShared<FWaveContainerNode>();
		waveContainerNode->WaveContainerName = DataWaveContainer->GetName();
		waveContainerNode->ContainerDescription = DataWaveContainer->DescriptionOfContainer;
		waveContainerNode->WaveContainerID = DataWaveContainer->WaveContainerID;
		for (int32 i = 0; i < DataWaveContainer->WavesArray.Num(); i++)
		{
			TSharedPtr<FWavesArrayNode> wavesArrayNode = MakeShared<FWavesArrayNode>();
			wavesArrayNode->Description = DataWaveContainer->WavesArray[i].WaveSettings.Description;
			wavesArrayNode->TotalPoints = DataWaveContainer->WavesArray[i].WaveSettings.TotalPoints;
			wavesArrayNode->WaveID = DataWaveContainer->WavesArray[i].WaveSettings.WaveID;
			wavesArrayNode->SpawnOrder = DataWaveContainer->WavesArray[i].SpawnOrder;
			for (int32 j = 0; j < DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes.Num(); j++)
			{
				TSharedPtr<FEnemyTypesNode> enemyTypesNode = MakeShared<FEnemyTypesNode>();
				enemyTypesNode->Cost = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].Cost;
				enemyTypesNode->EnemyClass = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].EnemyClass;
				enemyTypesNode->EnemyType = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].EnemyType;

				wavesArrayNode->EnemyTypes.Add(enemyTypesNode);
				wavesArrayNode->Children.Add(enemyTypesNode);

				/*FEnemyTypesNode enemyTypesNode(
					&DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].EnemyClass,
					DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].Cost,
					DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].EnemyType
				);*/
			}
			waveContainerNode->WavesArray.Add(wavesArrayNode);
			waveContainerNode->Children.Add(wavesArrayNode);
		}
		WaveContainerNodes.Add(waveContainerNode);
		RootNodes.Add(waveContainerNode);
	}

	// Display the data
	PropertyList->AddSlot()
	[
		SNew(STreeView<TSharedPtr<FBaseTreeNode>>)
			.TreeItemsSource(&RootNodes)
			.OnGenerateRow(this, &SWaveToolWidgetMenu::OnGenerateRow)
			.OnGetChildren(this, &SWaveToolWidgetMenu::OnGetChildren)
	];
}

TSharedRef<ITableRow> SWaveToolWidgetMenu::OnGenerateRow(TSharedPtr<FBaseTreeNode> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	switch (InItem->GetNodeType())
	{
	case ETreeNodeType::WaveContainer:
	{
		TSharedPtr<FWaveContainerNode> ContainerNode = StaticCastSharedPtr<FWaveContainerNode>(InItem);
		//int32 containerIndex = WaveContainerNodes.IndexOfByKey(MakeShared<FWaveContainerNode>(ContainerNode));
		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable)
		[
			SNew(SVerticalBox) + SVerticalBox::Slot()
			[
				SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
				[
					SNew(STextBlock).Text(FText::FromString(ContainerNode->WaveContainerName))
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(FMargin(10, 0))
				[
					SNew(SButton)
						.Text(FText::FromString(TEXT("-")))
						.OnClicked(this, &SWaveToolWidgetMenu::OnMinusButtonPressed, ContainerNode->WaveContainerName)
				]
			]
			+ SVerticalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SButton)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.VAlign(EVerticalAlignment::VAlign_Center)
					.Text(FText::FromString(TEXT("SAVE")))
					.OnClicked(this, &SWaveToolWidgetMenu::OnWaveContainerSaveButtonClicked, ContainerNode)
			]
		];
		break;
	}
	case ETreeNodeType::WavesArray:
	{
		TSharedPtr<FWavesArrayNode> WaveNode = StaticCastSharedPtr<FWavesArrayNode>(InItem);
		int32 containerIndex;
		int32 waveIndex;
		for (containerIndex = 0; containerIndex < WaveContainerNodes.Num(); containerIndex++)
		{
			//waveIndex = WaveContainerNodes[containerIndex]->WavesArray.IndexOfByKey(WaveNode);
			waveIndex = WaveContainerNodes[containerIndex]->WavesArray.IndexOfByPredicate([&](const TSharedPtr<FWavesArrayNode>& Node)
				{
					return Node == WaveNode;
				});
			if (waveIndex != INDEX_NONE)
				break;
		}

		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable)
		[
			SNew(SVerticalBox) + 
				SVerticalBox::Slot().AutoHeight()
				[
					SNew(SHorizontalBox) + SHorizontalBox::Slot()
					[
						SNew(STextBlock).Text(FText::FromString(TEXT("Wave ID")))
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SSpinBox<int32>)	// This is the numeric editable field with scroll
							//.OnValueChanged(this, &SWaveToolWidgetMenu::OnWaveIDValueChange, containerIndex, waveIndex)
							.OnValueChanged_Lambda([this, containerIndex, waveIndex](const int32 NewValue)
								{
									OnWaveIDValueChange(NewValue, containerIndex, waveIndex);
								})
							.MinValue(0)
							.Value(GetWaveIDEditableText(containerIndex, waveIndex))
					]
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SHorizontalBox) + SHorizontalBox::Slot()
					[
						SNew(STextBlock).Text(FText::FromString(TEXT("Wave Description")))
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SEditableTextBox)
							//.OnTextChanged(this, &SWaveToolWidgetMenu::OnWaveDecriptionTextChanged, containerIndex, waveIndex)
							.OnTextChanged_Lambda([this, containerIndex, waveIndex](const FText& NewText)
								{
									OnWaveDecriptionTextChanged(NewText, containerIndex, waveIndex);
								})
							.Text(GetWaveDescriptionEditableText(containerIndex, waveIndex))
					]
				] 
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SHorizontalBox) + SHorizontalBox::Slot()
					[
						SNew(STextBlock).Text(FText::FromString(TEXT("Total Points")))
					]
						+ SHorizontalBox::Slot()
					[
						SNew(SSpinBox<int32>)	// This is the numeric editable field with scroll
							//.OnValueChanged(this, &SWaveToolWidgetMenu::OnWaveTotalPointsTextChanged, containerIndex, waveIndex)
							.OnValueChanged_Lambda([this, containerIndex, waveIndex](const int32 NewValue)
								{
									OnWaveTotalPointsTextChanged(NewValue, containerIndex, waveIndex);
								})
							.MinValue(0)
							.Value(GetWaveTotalPointsEditableText(containerIndex, waveIndex))
					]
				]
		];
		break;
	}
	case ETreeNodeType::EnemyType:
	{
		TSharedPtr<FEnemyTypesNode> EnemyType = StaticCastSharedPtr<FEnemyTypesNode>(InItem);
		int32 containerIndex;
		int32 waveIndex;
		int32 enemyTypeIndex;
		for (containerIndex = 0; containerIndex < WaveContainerNodes.Num(); containerIndex++)
		{
			for (waveIndex = 0; waveIndex < WaveContainerNodes[containerIndex]->WavesArray.Num(); waveIndex++)
			{
				//enemyTypeIndex = WaveContainerNodes[containerIndex]->WavesArray.IndexOfByKey(EnemyType);
				enemyTypeIndex = WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes.IndexOfByPredicate(
					[&](const TSharedPtr<FEnemyTypesNode>& Node)
					{
						return Node == EnemyType;
					});
				if (enemyTypeIndex != INDEX_NONE)
					break;
			}
			if (enemyTypeIndex != INDEX_NONE)
				break;
		}

		TSharedPtr<FEnemyTypesNode> EnemyNode = StaticCastSharedPtr<FEnemyTypesNode>(InItem);
		FString EnemyClassName = EnemyNode->EnemyClass->GetName();
		FString EnemyTypeName = *UEnum::GetDisplayValueAsText(EnemyNode->EnemyType).ToString();
		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable)
		[
			SNew(SVerticalBox) +
			SVerticalBox::Slot().AutoHeight()
			[
				SNew(SHorizontalBox) + SHorizontalBox::Slot()
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("Enemy Class")))
				]
				// TODO CLASS LIST SELECTION MENU //
				/*	+ SHorizontalBox::Slot()
				[
					SNew(SEditableTextBox)
						.OnTextChanged(this, &SWaveToolWidgetMenu::OnWaveDecriptionTextChanged)
				]*/
			] + SVerticalBox::Slot().AutoHeight()
			[
				SNew(SHorizontalBox) + SHorizontalBox::Slot()
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("Enemy Cost")))
				]
				+ SHorizontalBox::Slot()
				[
					SNew(SSpinBox<int32>)
						//.OnValueChanged(this, &SWaveToolWidgetMenu::OnWaveEnemyCostTextChanged, containerIndex, waveIndex, enemyTypeIndex)
						.OnValueChanged_Lambda([this, containerIndex, waveIndex, enemyTypeIndex](const int32 NewValue)
							{
								OnWaveEnemyCostTextChanged(NewValue, containerIndex, waveIndex, enemyTypeIndex);
							})
						.MinValue(0)
						.Value(GetWaveEnemyCostEditableText(containerIndex, waveIndex, enemyTypeIndex))
				]
			] + SVerticalBox::Slot().AutoHeight()
			[
				SNew(SHorizontalBox) + SHorizontalBox::Slot()
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("Enemy Type")))
				]
				// ENUM DROP DOWN MENU //
				/*+ SHorizontalBox::Slot()
				[
					SNew(SComboBox<TSharedPtr<EEnemyTypes>>)
						.OptionsSource(&Options)
				]*/
			]
		];
		break;
	}
	default:
		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable);
		break;
	}
}

void SWaveToolWidgetMenu::OnGetChildren(TSharedPtr<FBaseTreeNode> InItem, TArray<TSharedPtr<FBaseTreeNode>>& OutChildren)
{
	if (!InItem.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnGetChildren called with an invalid InItem."));
		OutChildren.Empty();
		return;
	}

	switch (InItem->GetNodeType())
	{
	case ETreeNodeType::WaveContainer:
	{
		TSharedPtr<FWaveContainerNode> ContainerNode = StaticCastSharedPtr<FWaveContainerNode>(InItem);
		OutChildren = ContainerNode->Children;
		break;
	}
	case ETreeNodeType::WavesArray:
	{
		TSharedPtr<FWavesArrayNode> ContainerNode = StaticCastSharedPtr<FWavesArrayNode>(InItem);
		OutChildren = ContainerNode->Children;
		break;
	}
	case ETreeNodeType::EnemyType:
	{
		OutChildren.Empty();
		break;
	}
	};
	// I GUESS IT'S SOMETHING IN HERE FOR THE REFRESH //
}

FReply SWaveToolWidgetMenu::OnMinusButtonPressed(FString assetName)
{
	bool bDeleted = DeleteDataAsset(assetName);;

	RefreshDisplayedDataAssets();

	if (bDeleted)
		return FReply::Handled();
	else
		return FReply::Unhandled();
}

bool SWaveToolWidgetMenu::DeleteDataAsset(FString assetName)
{
	FString assetPathToDelete = "";
	assetPathToDelete.Append(WaveContainerPath);
	assetPathToDelete.Append(assetName);
	//FText text = FText::FromString(FString::Printf("Deleted asset at path: %s", assetPathToDelete));
	bool bDeleted = UEditorAssetLibrary::DeleteAsset(assetPathToDelete);
	if (bDeleted)
	{
		UE_LOG(LogTemp, Log, TEXT("Deleted asset at path : %s"), *FString(assetPathToDelete));

		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		AssetRegistryModule.Get().ScanPathsSynchronous({ assetPathToDelete });

		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	}
	return bDeleted;
}

void SWaveToolWidgetMenu::RefreshDisplayedDataAssets()
{
	DataWaveContainersArray.Empty();
	PropertyList->ClearChildren();
	RootNodes.Empty();

	GenerateDataTableWidget();
}

FReply SWaveToolWidgetMenu::OnSetWaveDataButtonClicked()
{
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Warning, TEXT("Set Wave Data"));
#endif
	//APWaveManager::Instance->SetWaveData(DataWaveContainer, false);
	return FReply::Handled();
}

TArray<FString> SWaveToolWidgetMenu::GetAllWaveDataAssetPaths()
{
	TArray<FString> WaveDataAssetPaths;

	WaveDataAssetPaths = UEditorAssetLibrary::ListAssets(TEXT("/Game/Custom/WaveSystem/WaveContainers"), true, false);
	return WaveDataAssetPaths;
}

TArray<UPDataWaveContainer*> SWaveToolWidgetMenu::GetDataWaveContainersFromPaths()
{
	TArray<FString> WaveDataAssetPaths = GetAllWaveDataAssetPaths();
	if (WaveDataAssetPaths.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No WaveData Assets found"));
		return TArray<UPDataWaveContainer*>();
	}
	for (FString assetPath : WaveDataAssetPaths)
	{
		UPDataWaveContainer* DataWaveContainer = Cast<UPDataWaveContainer>(UEditorAssetLibrary::LoadAsset(assetPath));
		if (DataWaveContainer)
		{
			DataWaveContainersArray.Add(DataWaveContainer);
		}
	}
	return DataWaveContainersArray;
}

#pragma region EDITABLE VALUES
//wave container
FText SWaveToolWidgetMenu::GetWaveContainerIDEditableText(const int32 containerIndex) const
{
	return FText::FromString(FString::Printf(TEXT("%d"), WaveContainerNodes[containerIndex]->WaveContainerID));
}

void SWaveToolWidgetMenu::OnWaveContainerIDChanged(const int32 NewValue, const int32 containerIndex)
{
	WaveContainerNodes[containerIndex]->WaveContainerID = NewValue;
}

FText SWaveToolWidgetMenu::GetWaveContainerDescriptionEditableText(const int32 containerIndex) const
{
	return FText::FromString(WaveContainerNodes[containerIndex]->ContainerDescription);
}

void SWaveToolWidgetMenu::OnWaveContainerDescriptionChanged(const FText& NewText, int32 containerIndex)
{
	WaveContainerNodes[containerIndex]->ContainerDescription = NewText.ToString();
}

//wave
int32 SWaveToolWidgetMenu::GetWaveIDEditableText(const int32 containerIndex, const int32 waveIndex) const
{
	return WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->WaveID;
}

void SWaveToolWidgetMenu::OnWaveIDValueChange(const int32 NewValue, const int32 containerIndex, const int32 waveIndex)
{
	WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->WaveID = NewValue;
}

FText SWaveToolWidgetMenu::GetWaveDescriptionEditableText(const int32 containerIndex, const int32 waveIndex) const
{
	return FText::FromString(WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->Description);
}

void SWaveToolWidgetMenu::OnWaveDecriptionTextChanged(const FText& NewText, const int32 containerIndex, const int32 waveIndex)
{
	WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->Description = NewText.ToString();
}

int32 SWaveToolWidgetMenu::GetWaveTotalPointsEditableText(const int32 containerIndex, const int32 waveIndex) const
{
	return WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->TotalPoints;
}

void SWaveToolWidgetMenu::OnWaveTotalPointsTextChanged(const int32 NewValue, const int32 containerIndex, const int32 waveIndex)
{
	WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->TotalPoints = NewValue;
}

FText SWaveToolWidgetMenu::GetWaveSpawnOrderEditableText(int32 containerIndex, const int32 waveIndex) const
{
	const UEnum* enumptr = StaticEnum<EEnemyTypes>();
	FString str = enumptr ? 
		enumptr->GetDisplayNameTextByValue((int64)WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->SpawnOrder).ToString()
		: TEXT("Invalid");
	FText txt = FText::FromString(str);
	return txt;
}

void SWaveToolWidgetMenu::OnWaveSpawnOrdertextChanged(const ESpawnOrder NewValue, const int32 containerIndex, const int32 waveIndex)
{
	WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->SpawnOrder = NewValue;
}

//enemy type
// CLASS SELECTION //
//FText SWaveToolWidgetMenu::GetWaveEnemyClassEditableText(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const
//{
//	return FText::FromString(WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyClass->Get()->GetName());
//}
//
//void SWaveToolWidgetMenu::OnWaveEnemyClassTextChanged(const TSubclassOf<AActor> NewValue, const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex)
//{
//}

int32 SWaveToolWidgetMenu::GetWaveEnemyCostEditableText(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const
{
	return WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->Cost;
}

void SWaveToolWidgetMenu::OnWaveEnemyCostTextChanged(const int32 NewValue, const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex)
{
	WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->Cost = NewValue;
}

FText SWaveToolWidgetMenu::GetWaveEnemyTypeEditableText(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const
{
	const UEnum* enumptr = StaticEnum<EEnemyTypes>();
	FString str = enumptr ?
		enumptr->GetDisplayNameTextByValue((int64)WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyType).ToString()
		: TEXT("Invalid");
	FText txt = FText::FromString(str);
	return txt;
}

void SWaveToolWidgetMenu::OnWaveEnemyTypeTextChanged(const EEnemyTypes NewValue, const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex)
{
	WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyType = NewValue;
}
#pragma endregion

FReply SWaveToolWidgetMenu::OnWaveContainerSaveButtonClicked(TSharedPtr<FWaveContainerNode> containerNode)
{
	// retrieve data wave asset from tarray//get wavedataassetcontainer from path
	//modify its value
	//save it
	UE_LOG(LogTemp, Log, TEXT("SAVE BUTTON PRESSED"));
	//int32 index = DataWaveContainersArray.IndexOfByKey(containerNode); can't use this since TSharedPtr doesn't have a '==' operator
	int32 index = WaveContainerNodes.IndexOfByPredicate([&](const TSharedPtr<FWaveContainerNode>& Node)
		{
			return Node == containerNode;
		});
	if (index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Didn't find WaveContainer in the database"));
		return FReply::Unhandled();
	}
	else
	{
		FString path = WaveContainerPath + WaveContainerNodes[index]->WaveContainerName;
		if (UEditorAssetLibrary::DoesAssetExist(path))
		{
			DeleteDataAsset(WaveContainerNodes[index]->WaveContainerName);
			//TODO maybe delete the asset from my list as well
		}
		//create a new UPDataWaveAsset and copy all the values from the corresponding WaveContainerNode
		UPDataWaveContainer* NewDataAsset = NewObject<UPDataWaveContainer>();	// since UPDataWaveContainer is a UObject
		NewDataAsset->Rename(*WaveContainerNodes[index]->WaveContainerName);
		//modify the values of the container
		//save the container
		NewDataAsset->WaveContainerID = WaveContainerNodes[index]->WaveContainerID;
		NewDataAsset->DescriptionOfContainer = WaveContainerNodes[index]->ContainerDescription;
		int32 waveArrayIndex = 0;
		for (auto& wave : WaveContainerNodes[index]->WavesArray)
		{
			FWaveOrder Wave;
			Wave.WaveSettings.WaveID = wave->WaveID;
			Wave.WaveSettings.Description = wave->Description;
			Wave.WaveSettings.TotalPoints = wave->TotalPoints;
			Wave.SpawnOrder = wave->SpawnOrder;
			for (auto& enemyType : wave->EnemyTypes)
			{
				FInternalDumbEnemyType EnemyType;
				EnemyType.EnemyClass = enemyType->EnemyClass;
				EnemyType.Cost = enemyType->Cost;
				EnemyType.EnemyType = enemyType->EnemyType;
				Wave.WaveSettings.EnemyTypes.Add(EnemyType);
			}
			NewDataAsset->WavesArray.Add(Wave);
			waveArrayIndex++;
		}
		SaveWaveDataAsset(NewDataAsset);
		return FReply::Handled();
	}
}

FReply SWaveToolWidgetMenu::OnPlusButtonClicked()
{
	auto container = NewObject<UPDataWaveContainer>();
	SaveWaveDataAsset(container);
	RefreshDisplayedDataAssets();

	return FReply::Handled();
}

FReply SWaveToolWidgetMenu::SaveWaveDataAsset(UPDataWaveContainer* container)
{
	FString containerName = container->GetName();

	// Create the package
	FString FullPackageName = WaveContainerPath + containerName;
	UPackage* Package = container->GetOutermost();
	if (!Package || Package->GetName() != FullPackageName)
	{
		Package = CreatePackage(*FullPackageName);
		if (!Package)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create package"));
			return FReply::Unhandled();
		}
		container->Rename(*containerName, Package, REN_DontCreateRedirectors);
	}

	container->SetFlags(RF_Public | RF_Standalone);
	container->MarkPackageDirty();
	
	FAssetRegistryModule::AssetCreated(container);

	FString PackageFileName = FPackageName::LongPackageNameToFilename(FullPackageName, FPackageName::GetAssetPackageExtension());

	bool bSaved = UPackage::SavePackage(
		Package, 
		container, 
		EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, 
		*PackageFileName,
		GError,
		nullptr,
		true,
		true,
		SAVE_NoError
	);

	if (bSaved)
	{
		UE_LOG(LogTemp, Warning, TEXT("Saved package %s"), *PackageFileName);
		return FReply::Handled();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to save package %s"), *PackageFileName);
		return FReply::Unhandled();
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
/*
//Selectable drop down menu//
Options.Add(MakeShared<FString>("Wawa 1"));
Options.Add(MakeShared<FString>("Wawa 2"));
Options.Add(MakeShared<FString>("Wawa 3"));

PropertyList->AddSlot()
[
	SNew(SComboBox<TSharedPtr<FString>>)
	.OptionsSource(&Options)
	.OnGenerateWidget(this, &SWaveToolWidgetMenu::HandleGenerateWidget)
	.OnSelectionChanged(this, &SWaveToolWidgetMenu::OnSelectionChanged)
	.InitiallySelectedItem(SelectedOption)
	[
		SNew(STextBlock)
		.Text_Lambda([this]() -> FText {
		return SelectedOption.IsValid() ? FText::FromString(*SelectedOption) : FText::FromString(TEXT("Select an option"));
		})
	]
];
TSharedRef<SWidget> SWaveToolWidgetMenu::HandleGenerateWidget(TSharedPtr<FString> InItem)
{
	return SNew(STextBlock)
		.Text(FText::FromString(*InItem));
}
void SWaveToolWidgetMenu::OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo)
{
	SelectedOption = NewValue;
}
//.h
TSharedPtr<SComboBox<TSharedPtr<FString>>> ComboBox;
TArray<TSharedPtr<FString>> Options;
TSharedPtr<FString> SelectedOption;

TSharedRef<SWidget> HandleGenerateWidget(TSharedPtr<FString> InItem);
void OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo);


// ON TEXT COMMITTED //
void SWaveToolWidgetMenu::OnWaveIDTextCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		UE_LOG(LogTemp, Warning, TEXT("User Entered: %s"), *NewText.ToString());
		//EditDataWaveContainer();
	}
}

why using the ids for the value setting, when it requires the user to set them up specifically otherwise the whole thing doesn't work properly

TODO: plus button next to wave array and enemytypes array to add a new one
*/