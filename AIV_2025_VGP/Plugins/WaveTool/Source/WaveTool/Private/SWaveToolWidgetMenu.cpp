// Fill out your copyright notice in the Description page of Project Settings.

#include "SWaveToolWidgetMenu.h"
#include "SlateOptMacros.h"
#include "PWaveManager.h"
#include <FileHelpers.h>
#include "EditorAssetLibrary.h"
#include <AssetRegistry/AssetRegistryModule.h>
#include "Widgets/Input/SSpinBox.h"
#include "PropertyCustomizationHelpers.h"
#include "UObject/Class.h"
#include "PackageTools.h"

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
					.VAlign(EVerticalAlignment::VAlign_Center).AutoHeight()
				[
					SNew(SButton)
						.Text(FText::FromString(TEXT("+"))).ToolTipText(FText::FromString(TEXT("Add new Wave Container")))
						.OnClicked(this, &SWaveToolWidgetMenu::OnWaveContainerPlusButtonClicked)
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
	EnemyTypes.Add(MakeShared<EEnemyTypes>(EEnemyTypes::Melee));
	EnemyTypes.Add(MakeShared<EEnemyTypes>(EEnemyTypes::Ranged));
	EnemyTypes.Add(MakeShared<EEnemyTypes>(EEnemyTypes::Tank));
	SelectedEnemyType = EnemyTypes[0];
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
	int32 l = 0;
	// Transfer data from the array of AssetDataWaveContainers to my custom struct
	TArray<TArray<TSharedPtr<FWaveContainerNode>>> RootNodesContainer;
	for (UPDataWaveContainer* DataWaveContainer : DataWaveContainersArray)
	{
		TSharedPtr<FWaveContainerNode> waveContainerNode = MakeShared<FWaveContainerNode>();
		waveContainerNode->WaveContainerName = DataWaveContainer->GetName();
		waveContainerNode->ContainerDescription = DataWaveContainer->DescriptionOfContainer;
		for (int32 i = 0; i < DataWaveContainer->WavesArray.Num(); i++)
		{
			TSharedPtr<FWavesArrayNode> wavesArrayNode = MakeShared<FWavesArrayNode>();
			wavesArrayNode->waveID = i;
			wavesArrayNode->waveContainerID = l;
			wavesArrayNode->Description = DataWaveContainer->WavesArray[i].WaveSettings.Description;
			wavesArrayNode->TotalPoints = DataWaveContainer->WavesArray[i].WaveSettings.TotalPoints;
			wavesArrayNode->SpawnOrder = DataWaveContainer->WavesArray[i].SpawnOrder;
			for (int32 j = 0; j < DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes.Num(); j++)
			{
				TSharedPtr<FEnemyTypesNode> enemyTypesNode = MakeShared<FEnemyTypesNode>();
				enemyTypesNode->EnemyTypesID = j;
				enemyTypesNode->waveID = i;
				enemyTypesNode->waveContainerID = l;
				enemyTypesNode->Cost = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].Cost;
				enemyTypesNode->EnemyClass = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].EnemyClass;
				enemyTypesNode->EnemyType = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].EnemyType;

				wavesArrayNode->EnemyTypes.Add(enemyTypesNode);
				wavesArrayNode->Children.Add(enemyTypesNode);
			}
			waveContainerNode->WavesArray.Add(wavesArrayNode);
			waveContainerNode->Children.Add(wavesArrayNode);
		}
		waveContainerNode->waveContainerID = l++;
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
		int32 containerIndex = WaveContainerNodes.IndexOfByKey(ContainerNode);
		int32 containerArrayIndex = DataWaveContainersArray.IndexOfByPredicate([&](const UPDataWaveContainer* container) {
			return container->GetName() == WaveContainerNodes[containerIndex]->WaveContainerName;
		});
		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable)
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				[
					SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
						[
							SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
							[
								SNew(STextBlock).Text(FText::FromString(ContainerNode->WaveContainerName))
							]
								+ SHorizontalBox::Slot().AutoWidth().Padding(FMargin(10, 0))
							[
								SNew(SEditableTextBox)
									.OnTextChanged_Lambda([this, containerIndex](const FText& NewText)
										{
											OnWaveContainerNameChanged(NewText, containerIndex);
										})
									.Text(GetWaveContainerNameEditableText(containerIndex))
							]
						]
						+ SHorizontalBox::Slot().AutoWidth().Padding(FMargin(10, 0))
						[
							SNew(SButton)
								.Text(FText::FromString(TEXT("-"))).ToolTipText(FText::FromString(TEXT("Remove Wave Container")))
								.OnClicked(this, &SWaveToolWidgetMenu::OnMinusButtonPressed, ContainerNode->WaveContainerName)
						]
				]
				+ SVerticalBox::Slot()
				[
					SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
					[
						SNew(STextBlock).Text(FText::FromString(TEXT("Wave Container Description")))
					]
					+ SHorizontalBox::Slot().AutoWidth().Padding(10, 0)
					[
						SNew(SEditableTextBox)
							.OnTextChanged_Lambda([this, containerIndex](const FText& NewText)
								{
									OnWaveContainerDescriptionChanged(NewText, containerIndex);
								})
							.Text(GetWaveContainerDescriptionEditableText(containerIndex))
					]
				]
				+ SVerticalBox::Slot()
				[
					SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
					[
						SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Waves N.  %d"), ContainerNode->WavesArray.Num())))
					]
					+ SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Left)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.AutoWidth()
						.Padding(10, 0)
					[
						SNew(SButton)
							.Text(FText::FromString(TEXT("+"))).ToolTipText(FText::FromString(TEXT("Add new Wave")))
							.OnClicked(this, &SWaveToolWidgetMenu::OnWavesArrayPlusButtonClicked, containerArrayIndex)
							.HAlign(EHorizontalAlignment::HAlign_Center)
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
						.ToolTipText(FText::FromString(TEXT("Save Wave Container")))
				]
		];
		break;
	}
	case ETreeNodeType::WavesArray:
	{
		TSharedPtr<FWavesArrayNode> WaveNode = StaticCastSharedPtr<FWavesArrayNode>(InItem);
		//int32 containerIndex;
		//int32 waveIndex;
		//for (containerIndex = 0; containerIndex < WaveContainerNodes.Num(); containerIndex++)
		//{
		//	//waveIndex = WaveContainerNodes[containerIndex]->WavesArray.IndexOfByKey(WaveNode);
		//	waveIndex = WaveContainerNodes[containerIndex]->WavesArray.IndexOfByPredicate([&](const TSharedPtr<FWavesArrayNode>& Node) {
		//		return Node == WaveNode;
		//	});
		//	if (waveIndex != INDEX_NONE)
		//		break;
		//}
		///*int32 waveArrayIndex = DataWaveContainersArray[containerArrayIndex]->WavesArray.IndexOfByPredicate([&]() {
		//		return 
		//})*/
		//int32 containerArrayIndex = DataWaveContainersArray.IndexOfByPredicate([&](const UPDataWaveContainer* container) {
		//	return container->GetName() == WaveContainerNodes[containerIndex]->WaveContainerName;
		//});

		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable)
		[
			SNew(SBox).Padding(0, 5)
			[
				SNew(SBorder).Padding(5, 7)
				[
					SNew(SHorizontalBox) + SHorizontalBox::Slot()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight()
						[
							SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Wave N.  %d"), WaveNode->waveID)))
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
								.OnTextChanged_Lambda([this, WaveNode](const FText& NewText)
								{
									OnWaveDecriptionTextChanged(NewText, WaveNode->waveContainerID, WaveNode->waveID);
								})
								.Text(GetWaveDescriptionEditableText(WaveNode->waveContainerID, WaveNode->waveID))
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
								.OnValueChanged_Lambda([this, WaveNode](const int32 NewValue)
									{
										OnWaveTotalPointsTextChanged(NewValue, WaveNode->waveContainerID, WaveNode->waveID);
									})
								.MinValue(0)
								.Value(GetWaveTotalPointsEditableText(WaveNode->waveContainerID, WaveNode->waveID))
							]
						]
						+ SVerticalBox::Slot()
						[
							SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
							[
								SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Enemy Types N.  %d"), WaveNode->EnemyTypes.Num())))
							]
							+ SHorizontalBox::Slot()
							.HAlign(EHorizontalAlignment::HAlign_Left)
							.VAlign(EVerticalAlignment::VAlign_Center)
							.AutoWidth()
							.Padding(10, 0)
							[
								SNew(SButton)
									.Text(FText::FromString(TEXT("+"))).ToolTipText(FText::FromString(TEXT("Add new Enemy Type")))
									.OnClicked(this, &SWaveToolWidgetMenu::OnEnemyTypesPlusButtonClicked, WaveNode->waveContainerID, WaveNode->waveID)
									.HAlign(EHorizontalAlignment::HAlign_Center)
							]
						]
					]
					+ SHorizontalBox::Slot().VAlign(EVerticalAlignment::VAlign_Center).HAlign(EHorizontalAlignment::HAlign_Right).Padding(0, 0, 15, 0)
					[
						SNew(SButton)
							.Text(FText::FromString(TEXT("-")))
							.ToolTipText(FText::FromString(TEXT("Remove Wave")))
							.OnClicked(this, &SWaveToolWidgetMenu::OnWavesArrayMinusButtonClicked, WaveNode->waveContainerID, WaveNode->waveID)
					]
				]
			]
		];
		break;
	}
	case ETreeNodeType::EnemyType:
	{
		TSharedPtr<FEnemyTypesNode> EnemyType = StaticCastSharedPtr<FEnemyTypesNode>(InItem);
		//int32 containerIndex;
		//int32 waveIndex;
		//int32 enemyTypeIndex;
		//for (containerIndex = 0; containerIndex < WaveContainerNodes.Num(); containerIndex++)
		//{
		//	for (waveIndex = 0; waveIndex < WaveContainerNodes[containerIndex]->WavesArray.Num(); waveIndex++)
		//	{
		//		//enemyTypeIndex = WaveContainerNodes[containerIndex]->WavesArray.IndexOfByKey(EnemyType);
		//		enemyTypeIndex = WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes.IndexOfByPredicate(
		//			[&](const TSharedPtr<FEnemyTypesNode>& Node)
		//			{
		//				return Node == EnemyType;
		//			});
		//		if (enemyTypeIndex != INDEX_NONE)
		//			break;
		//	}
		//	if (enemyTypeIndex != INDEX_NONE)
		//		break;
		//}


		//int32 arrayContainerIndex = DataWaveContainersArray.IndexOfByPredicate([this, containerIndex](const UPDataWaveContainer* container) {
		//	return container->GetName() == WaveContainerNodes[containerIndex]->WaveContainerName;
		//});
		//int32 arrayWaveIndex = 0;
		//int32 arrayenemyTypeIndex = 0;
		//bool check = true;
		//while (arrayWaveIndex < DataWaveContainersArray[arrayContainerIndex]->WavesArray.Num())
		//{
		//	while (arrayenemyTypeIndex < DataWaveContainersArray[arrayContainerIndex]->WavesArray[arrayWaveIndex].WaveSettings.EnemyTypes.Num())
		//	{
		//		if (DataWaveContainersArray[arrayContainerIndex]->WavesArray[arrayWaveIndex].WaveSettings.EnemyTypes[arrayenemyTypeIndex].Cost ==
		//			WaveContainerNodes[arrayContainerIndex]->WavesArray[arrayWaveIndex]->EnemyTypes[arrayenemyTypeIndex]->Cost &&
		//			DataWaveContainersArray[arrayContainerIndex]->WavesArray[arrayWaveIndex].WaveSettings.EnemyTypes[arrayenemyTypeIndex].EnemyType ==
		//			WaveContainerNodes[arrayContainerIndex]->WavesArray[arrayWaveIndex]->EnemyTypes[arrayenemyTypeIndex]->EnemyType &&
		//			DataWaveContainersArray[arrayContainerIndex]->WavesArray[arrayWaveIndex].WaveSettings.EnemyTypes[arrayenemyTypeIndex].EnemyClass ==
		//			WaveContainerNodes[arrayContainerIndex]->WavesArray[arrayWaveIndex]->EnemyTypes[arrayenemyTypeIndex]->EnemyClass)
		//		{
		//			check = false;
		//			break;
		//		}
		//		else
		//			arrayenemyTypeIndex++;
		//	}
		//	if (check)
		//		arrayWaveIndex++;
		//	else
		//		break;
		//}
		/*WaveContainerNodes[arrayContainerIndex]->WavesArray[arrayWaveIndex]->EnemyTypes[enemyTypeIndex]->EnemyClass = 
			DataWaveContainersArray[arrayContainerIndex]->WavesArray[arrayWaveIndex].WaveSettings.EnemyTypes[arrayenemyTypeIndex].EnemyClass;
		SelectedEnemyType = MakeShared<EEnemyTypes>
			(DataWaveContainersArray[arrayContainerIndex]->WavesArray[arrayWaveIndex].WaveSettings.EnemyTypes[arrayenemyTypeIndex].EnemyType);*/
		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable)
		[
			SNew(SBox).Padding(0, 5)
			[
				SNew(SBorder).Padding(5, 7)
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight()
						[
							SNew(SHorizontalBox) + SHorizontalBox::Slot()
								[
									SNew(STextBlock).Text(FText::FromString(TEXT("Enemy Class")))
								]
								// CLASS LIST SELECTION MENU //
								+ SHorizontalBox::Slot()
								[
									SNew(SClassPropertyEntryBox)
										.MetaClass(AActor::StaticClass())
										.SelectedClass(this, &SWaveToolWidgetMenu::GetSelectedClass, EnemyType->waveContainerID, EnemyType->waveID, EnemyType->EnemyTypesID)
										.AllowNone(true)
										.OnSetClass_Lambda([this, EnemyType](const UClass* SelectedClass) {
										this->OnClassSelected(SelectedClass, EnemyType->waveContainerID, EnemyType->waveID, EnemyType->EnemyTypesID);
											})
								]
						]
						+ SVerticalBox::Slot().AutoHeight()
						[
							SNew(SHorizontalBox) + SHorizontalBox::Slot()
								[
									SNew(STextBlock).Text(FText::FromString(TEXT("Enemy Cost")))
								]
								+ SHorizontalBox::Slot()
								[
									SNew(SSpinBox<int32>)
										.OnValueChanged_Lambda([this, EnemyType](const int32 NewValue)
											{
												OnWaveEnemyCostTextChanged(NewValue, EnemyType->waveContainerID, EnemyType->waveID, EnemyType->EnemyTypesID);
											})
										.MinValue(0)
										.Value(GetWaveEnemyCostEditableText(EnemyType->waveContainerID, EnemyType->waveID, EnemyType->EnemyTypesID))
								]
						]
						+ SVerticalBox::Slot().AutoHeight()
						[
							SNew(SHorizontalBox) + SHorizontalBox::Slot()
								[
									SNew(STextBlock).Text(FText::FromString(TEXT("Enemy Type")))
								]
								// ENUM DROP DOWN MENU //
								+ SHorizontalBox::Slot()
								[
									SNew(SComboBox<TSharedPtr<EEnemyTypes>>)
										.OptionsSource(&EnemyTypes)
										.OnGenerateWidget(this, &SWaveToolWidgetMenu::GenerateComboItem)
										.OnSelectionChanged_Lambda([this, EnemyType]
										(TSharedPtr<EEnemyTypes> NewValue, ESelectInfo::Type SelectInfo) {
												SWaveToolWidgetMenu::OnEnemyTypeSelectionChanged(NewValue, SelectInfo, EnemyType->waveContainerID, EnemyType->waveID, EnemyType->EnemyTypesID);
											})
										.InitiallySelectedItem(SelectedEnemyType)
										[
											SNew(STextBlock).Text(this, &SWaveToolWidgetMenu::GetCurrentItemLabel, EnemyType->waveContainerID, EnemyType->waveID, EnemyType->EnemyTypesID)
										]
								]
						]
				]
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
}

FReply SWaveToolWidgetMenu::OnMinusButtonPressed(FString assetName)
{
	bool bDeleted = DeleteDataAsset(assetName);

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
	int32 index = DataWaveContainersArray.IndexOfByPredicate([assetName](const UPDataWaveContainer* container) {
		return container->GetName() == assetName;
	});
	bool bDeleted = UEditorAssetLibrary::DeleteAsset(assetPathToDelete);
	if (bDeleted)
	{
		UE_LOG(LogTemp, Log, TEXT("Deleted asset at path : %s"), *FString(assetPathToDelete));

		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		AssetRegistryModule.Get().ScanPathsSynchronous({ assetPathToDelete });

		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

		DataWaveContainersArray.RemoveAt(index);
	}
	return bDeleted;
}

void SWaveToolWidgetMenu::RefreshDisplayedDataAssets()
{
	DataWaveContainersArray.Empty();
	WaveContainerNodes.Empty();
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
// wave container
FText SWaveToolWidgetMenu::GetWaveContainerNameEditableText(const int32 containerIndex) const
{
	return FText::FromString(WaveContainerNodes[containerIndex]->WaveContainerName);
}

void SWaveToolWidgetMenu::OnWaveContainerNameChanged(const FText& NewText, int32 containerIndex)
{
	FString name = PackageTools::SanitizePackageName(NewText.ToString());
	WaveContainerNodes[containerIndex]->WaveContainerName = name;
}

FText SWaveToolWidgetMenu::GetWaveContainerDescriptionEditableText(const int32 containerIndex) const
{
	return FText::FromString(WaveContainerNodes[containerIndex]->ContainerDescription);
}

void SWaveToolWidgetMenu::OnWaveContainerDescriptionChanged(const FText& NewText, int32 containerIndex)
{
	WaveContainerNodes[containerIndex]->ContainerDescription = NewText.ToString();
}

// wave
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
	FString path = WaveContainerPath + WaveContainerNodes[containerNode->waveContainerID]->WaveContainerName;
	if (UEditorAssetLibrary::DoesAssetExist(path))
	{
		DeleteDataAsset(WaveContainerNodes[containerNode->waveContainerID]->WaveContainerName);
		//TODO maybe delete the asset from my list as well
	}
	//create a new UPDataWaveAsset and copy all the values from the corresponding WaveContainerNode
	UPDataWaveContainer* NewDataAsset = NewObject<UPDataWaveContainer>();	// since UPDataWaveContainer is a UObject
	NewDataAsset->Rename(*WaveContainerNodes[containerNode->waveContainerID]->WaveContainerName);
	//modify the values of the container
	//save the container
	NewDataAsset->DescriptionOfContainer = WaveContainerNodes[containerNode->waveContainerID]->ContainerDescription;
	int32 waveArrayIndex = 0;
	for (auto& wave : WaveContainerNodes[containerNode->waveContainerID]->WavesArray)
	{
		FWaveOrder Wave;
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
	RefreshDisplayedDataAssets();
	return FReply::Handled();
}

FReply SWaveToolWidgetMenu::OnWaveContainerPlusButtonClicked()
{
	auto container = NewObject<UPDataWaveContainer>();
	SaveWaveDataAsset(container);
	RefreshDisplayedDataAssets();

	return FReply::Handled();
}

FReply SWaveToolWidgetMenu::OnWavesArrayPlusButtonClicked(int32 containerIndex)
{
	FWaveOrder* wave = new FWaveOrder();
	DataWaveContainersArray[containerIndex]->WavesArray.Add(*wave);
	/*TSharedPtr<FWaveContainerNode> waveContainerNode = WaveContainerNodes[containerIndex];
	FWavesArrayNode* waveArrayNode = new FWavesArrayNode();
	waveContainerNode->WavesArray.Add(MakeShared<FWavesArrayNode>(*waveArrayNode));
	waveContainerNode->Children.Add(MakeShared<FWavesArrayNode>(*waveArrayNode));
	RootNodes[containerIndex] = waveContainerNode;*/
	//SaveWaveDataAsset(DataWaveContainersArray[containerIndex]);
	RefreshDisplayedDataAssets();
	return FReply::Handled();
}

FReply SWaveToolWidgetMenu::OnWavesArrayMinusButtonClicked(int32 containerIndex, int32 waveIndex)
{
	DataWaveContainersArray[containerIndex]->WavesArray.RemoveAt(waveIndex);
	SaveWaveDataAsset(DataWaveContainersArray[containerIndex]);
	RefreshDisplayedDataAssets();
	return FReply::Handled();
}

FReply SWaveToolWidgetMenu::OnEnemyTypesPlusButtonClicked(int32 containerIndex, int32 waveIndex)
{
	FInternalDumbEnemyType* enemyType = new FInternalDumbEnemyType();
	DataWaveContainersArray[containerIndex]->WavesArray[waveIndex].WaveSettings.EnemyTypes.Add(*enemyType);
	RefreshDisplayedDataAssets();
	return FReply::Handled();
}

FReply SWaveToolWidgetMenu::OnEnemyTypesMinusButtonClicked(int32 containerIndex, int32 waveIndex, int32 enemyTypeIndex)
{
	DataWaveContainersArray[containerIndex]->WavesArray[waveIndex].WaveSettings.EnemyTypes.RemoveAt(enemyTypeIndex);
	SaveWaveDataAsset(DataWaveContainersArray[containerIndex]);
	RefreshDisplayedDataAssets();
	return FReply::Handled();
}

// ENUM DROP DOWN MENU //
void SWaveToolWidgetMenu::OnEnemyTypeSelectionChanged(TSharedPtr<EEnemyTypes> NewValue, ESelectInfo::Type SelectInfo, int32 containerIndex, int32 waveIndex, int32 enemyTypeIndex)
{
	if (NewValue.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected: %d"), static_cast<int32>(*NewValue));
		SelectedEnemyType = NewValue;
		DataWaveContainersArray[containerIndex]->WavesArray[waveIndex].WaveSettings.EnemyTypes[enemyTypeIndex].EnemyType = *NewValue.Get();
		WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyType = *NewValue.Get();
	}
}

TSharedRef<SWidget> SWaveToolWidgetMenu::GenerateComboItem(TSharedPtr<EEnemyTypes> Item)
{
	return SNew(STextBlock).Text(EnumToText(*Item));
}

FText SWaveToolWidgetMenu::GetCurrentItemLabel(int32 containerIndex, int32 waveIndex, int32 enemyTypeIndex) const
{
	return EnumToText(WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyType);
}

FText SWaveToolWidgetMenu::EnumToText(EEnemyTypes EnumValue) const
{
	switch (EnumValue)
	{
	case EEnemyTypes::Melee:
		return FText::FromString("Melee");
		break;
	case EEnemyTypes::Ranged:
		return FText::FromString("Ranged");
		break;
	case EEnemyTypes::Tank:
		return FText::FromString("Tank");
		break;
	default:
		return FText::FromString("Unknown");
		break;
	}
}

// class selection
void SWaveToolWidgetMenu::OnClassSelected(const UClass* selectedClass, int32 containerIndex, int32 waveIndex, int32 enemyTypeIndex)
{
	if (!selectedClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No class selected or selection cleared."));
		return;
	}
	if (selectedClass->IsChildOf(UBlueprint::StaticClass()))
	{
		const UBlueprint* Blueprint = Cast<UBlueprint>(selectedClass);
		if (Blueprint && Blueprint->GeneratedClass && Blueprint->GeneratedClass->IsChildOf(AActor::StaticClass()))
		{
			WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyClass = Blueprint->GeneratedClass;

			UE_LOG(LogTemp, Log, TEXT("Stored Blueprint Actor Class: %s"),
				*WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyClass->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Blueprint class or not derived from AActor."));
		}
	}
	// Handle native C++ classes
	else if (selectedClass->IsChildOf(AActor::StaticClass()))
	{
		WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyClass = const_cast<UClass*>(selectedClass);
		UE_LOG(LogTemp, Log, TEXT("Stored Native Actor Class: %s"),
			*WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyClass->GetName());
		//DataWaveContainersArray[containerIndex]->WavesArray[waveIndex].WaveSettings.EnemyTypes[enemyTypeIndex].EnemyClass = SelectedActorClass;
		//WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyClass = SelectedActorClass;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected class is not a valid AActor subclass."));
	}

	/*UE_LOG(LogTemp, Log, TEXT("Selected Actor Class: %s"), *SelectedClass->GetName());
	TSubclassOf<AActor> selectedactor = const_cast<UClass*>(SelectedClass);
	WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->EnemyClass = selectedactor;
	SelectedClass = const_cast<UClass*>(selectedClass);*/
}

const UClass* SWaveToolWidgetMenu::GetSelectedClass(int32 containerIndex, int32 arrayWaveIndex, int32 enemyTypeIndex) const
{
	return WaveContainerNodes[containerIndex]->WavesArray[arrayWaveIndex]->EnemyTypes[enemyTypeIndex]->EnemyClass.Get();
}

FReply SWaveToolWidgetMenu::SaveWaveDataAsset(UPDataWaveContainer* container)
{
	FString containerName = container->GetName();

	// Create the package
	FString FullPackageName = WaveContainerPath + containerName;
	UPackage* Package = container->GetOutermost();
	FName::IsValidXName(FullPackageName, INVALID_OBJECTNAME_CHARACTERS);
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
* 
*/