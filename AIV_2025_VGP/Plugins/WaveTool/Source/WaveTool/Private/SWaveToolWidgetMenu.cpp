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
#include "EngineUtils.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SWaveToolWidgetMenu::Construct(const FArguments& InArgs)
{
	GenerateDataTableWidget();
	WidgetTab = EWidgetTab::WaveContainers;
	ChildSlot
	[
		SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth()
					[
						SNew(SBox).HAlign(EHorizontalAlignment::HAlign_Left).Padding(0, 6)
							[
								SNew(SButton).Text(FText::FromString(TEXT("DataWave Containers")))
									.OnClicked_Lambda([this]
										{
											WidgetTab = EWidgetTab::WaveContainers;
											return FReply::Handled();
										})
							]
					]
					+ SHorizontalBox::Slot().AutoWidth()
					[
						SNew(SBox).HAlign(EHorizontalAlignment::HAlign_Left).Padding(0, 6)
							[
								SNew(SButton).Text(FText::FromString(TEXT("Wave Managers"))).HAlign(EHorizontalAlignment::HAlign_Center)
									.OnClicked_Lambda([this]
										{
											WidgetTab = EWidgetTab::WaveManagers;
											return FReply::Handled();
										})
							]
					]
			]
			+ SVerticalBox::Slot()
			[
				SNew(SBorder)
					.Visibility_Lambda([this]
						{
							return (WidgetTab == EWidgetTab::WaveContainers) ? EVisibility::Visible : EVisibility::Collapsed;
						})
					[
						SNew(SVerticalBox) + SVerticalBox::Slot()
							.HAlign(EHorizontalAlignment::HAlign_Center)
							.VAlign(EVerticalAlignment::VAlign_Center).AutoHeight()
							[
								SNew(SBorder)
									.HAlign(EHorizontalAlignment::HAlign_Center)
									.VAlign(EVerticalAlignment::VAlign_Center)
									[
										SNew(SHorizontalBox)
											+ SHorizontalBox::Slot()
											.HAlign(EHorizontalAlignment::HAlign_Center)
											.VAlign(EVerticalAlignment::VAlign_Center)
											.AutoWidth()
											.Padding(10, 0)
											[
												SNew(STextBlock).Text(FText::FromString(TEXT("Create new Wave Data Asset")))
											]
											+ SHorizontalBox::Slot()
											[
												SNew(SButton)
													.Text(FText::FromString(TEXT("+"))).ToolTipText(FText::FromString(TEXT("Add new Wave Container")))
													.OnClicked_Lambda([this]()
														{
															auto container = NewObject<UPDataWaveContainer>();
															SaveWaveDataAsset(container);
															RefreshDisplayedDataAssets();

															return FReply::Handled();
														})
											]
									]
							]
							+ SVerticalBox::Slot()
							[
								SNew(STreeView<TSharedPtr<FBaseTreeNode>>)
									.TreeItemsSource(&RootNodes)
									.OnGenerateRow(this, &SWaveToolWidgetMenu::OnGenerateRow)
									.OnGetChildren(this, &SWaveToolWidgetMenu::OnGetChildren)
							]
					]
			]
			+ SVerticalBox::Slot()
			[
				SNew(SBorder).Visibility_Lambda([this]
					{
						return (WidgetTab == EWidgetTab::WaveContainers) ? EVisibility::Collapsed : EVisibility::Visible;
					})
					[
						SNew(STextBlock).Text(FText::FromString(TEXT("awawa")))
					]
			]
	];
	// ENUM DROP DOWN MENU STUFF //
	EnemyTypes.Add(MakeShared<EEnemyTypes>(EEnemyTypes::Melee));
	EnemyTypes.Add(MakeShared<EEnemyTypes>(EEnemyTypes::Ranged));
	EnemyTypes.Add(MakeShared<EEnemyTypes>(EEnemyTypes::Tank));
	SelectedEnemyType = EnemyTypes[0];
	SpawnOrders.Add(MakeShared<ESpawnOrder>(ESpawnOrder::Order1));
	SpawnOrders.Add(MakeShared<ESpawnOrder>(ESpawnOrder::Order2));
	SpawnOrders.Add(MakeShared<ESpawnOrder>(ESpawnOrder::Order3));
	SpawnOrders.Add(MakeShared<ESpawnOrder>(ESpawnOrder::Order4));
	SpawnOrders.Add(MakeShared<ESpawnOrder>(ESpawnOrder::Order5));
	SpawnOrders.Add(MakeShared<ESpawnOrder>(ESpawnOrder::Order6));
	SpawnOrders.Add(MakeShared<ESpawnOrder>(ESpawnOrder::Order7));
	SpawnOrders.Add(MakeShared<ESpawnOrder>(ESpawnOrder::Order8));
	SpawnOrders.Add(MakeShared<ESpawnOrder>(ESpawnOrder::Order9));
	SpawnOrders.Add(MakeShared<ESpawnOrder>(ESpawnOrder::Order10));
	SelectedSpawnOrder = SpawnOrders[0];
}

// Show properties, detached from the construct function
void SWaveToolWidgetMenu::GenerateDataTableWidget()
{
	DataWaveContainersArray = GetDataWaveContainersFromPaths();
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
			wavesArrayNode->WaveID = i;
			wavesArrayNode->WaveContainerID = l;
			wavesArrayNode->Description = DataWaveContainer->WavesArray[i].WaveSettings.Description;
			wavesArrayNode->TotalPoints = DataWaveContainer->WavesArray[i].WaveSettings.TotalPoints;
			wavesArrayNode->SpawnOrder = DataWaveContainer->WavesArray[i].SpawnOrder;
			for (int32 j = 0; j < DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes.Num(); j++)
			{
				TSharedPtr<FEnemyTypesNode> enemyTypesNode = MakeShared<FEnemyTypesNode>();
				enemyTypesNode->EnemyTypesID = j;
				enemyTypesNode->WaveID = i;
				enemyTypesNode->WaveContainerID = l;
				enemyTypesNode->Cost = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].Cost;
				enemyTypesNode->EnemyClass = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].EnemyClass;
				enemyTypesNode->UseEnemyCost = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].bUseEnemyCost;
				enemyTypesNode->MinEnemies = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].MinEnemies;
				enemyTypesNode->MaxEnemies = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].MaxEnemies;
				enemyTypesNode->EnemyType = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].EnemyType;

				wavesArrayNode->EnemyTypes.Add(enemyTypesNode);
				wavesArrayNode->Children.Add(enemyTypesNode);
			}
			waveContainerNode->WavesArray.Add(wavesArrayNode);
			waveContainerNode->Children.Add(wavesArrayNode);
		}
		waveContainerNode->WaveContainerID = l++;
		WaveContainerNodes.Add(waveContainerNode);
		RootNodes.Add(waveContainerNode);
	}
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
											FString name = PackageTools::SanitizePackageName(NewText.ToString());
											WaveContainerNodes[containerIndex]->WaveContainerName = name;
										})
									.Text_Lambda([this, containerIndex]()
										{
											return FText::FromString(WaveContainerNodes[containerIndex]->WaveContainerName);
										})
							]
						]
						+ SHorizontalBox::Slot().AutoWidth().Padding(FMargin(10, 0))
						[
							SNew(SButton)
								.Text(FText::FromString(TEXT("-")))
								.ToolTipText(FText::FromString(TEXT("Remove Wave Container")))
								.OnClicked_Lambda([this, ContainerNode]()
									{
										bool bDeleted = DeleteDataAsset(ContainerNode->WaveContainerName);

										RefreshDisplayedDataAssets();

										if (bDeleted)
										{
											return FReply::Handled();
										}
										else
										{
											return FReply::Unhandled();
										}
									})
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
									WaveContainerNodes[containerIndex]->ContainerDescription = NewText.ToString();
								})
							.Text_Lambda([this, containerIndex]()
								{
									return FText::FromString(WaveContainerNodes[containerIndex]->ContainerDescription);
								})
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
							.HAlign(EHorizontalAlignment::HAlign_Center)
							.OnClicked_Lambda([this, containerArrayIndex]()
								{
									FWaveOrder* wave = new FWaveOrder();
									DataWaveContainersArray[containerArrayIndex]->WavesArray.Add(*wave);
									RefreshDisplayedDataAssets();
									return FReply::Handled();
								})
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
							SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Wave N.  %d"), WaveNode->WaveID)))
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
									WaveContainerNodes[WaveNode->WaveContainerID]->WavesArray[WaveNode->WaveID]->Description = NewText.ToString();
								})
								.Text_Lambda([this, WaveNode]()
								{
									return FText::FromString(WaveContainerNodes[WaveNode->WaveContainerID]->WavesArray[WaveNode->WaveID]->Description);
								})
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
								.OnValueChanged_Lambda([this, WaveNode](const int32 NewValue)
									{
										WaveContainerNodes[WaveNode->WaveContainerID]->WavesArray[WaveNode->WaveID]->TotalPoints = NewValue;
									})
								.MinValue(0)
								.Value_Lambda([this, WaveNode]()
									{
										return WaveContainerNodes[WaveNode->WaveContainerID]->WavesArray[WaveNode->WaveID]->TotalPoints;
									})
							]
						]
						+ SVerticalBox::Slot()
						[
							SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
							[
								SNew(STextBlock)
									.Text(FText::FromString(FString::Printf(TEXT("Enemy Types N.  %d"), WaveNode->EnemyTypes.Num())))
							]
							+ SHorizontalBox::Slot()
								.HAlign(EHorizontalAlignment::HAlign_Left)
								.VAlign(EVerticalAlignment::VAlign_Center)
								.AutoWidth()
								.Padding(10, 0)
							[
								SNew(SButton)
									.Text(FText::FromString(TEXT("+")))
									.ToolTipText(FText::FromString(TEXT("Add new Enemy Type")))
									.HAlign(EHorizontalAlignment::HAlign_Center)
									.OnClicked_Lambda([this, WaveNode]()
										{
											FInternalDumbEnemyType* enemyType = new FInternalDumbEnemyType();
											DataWaveContainersArray[WaveNode->WaveContainerID]->WavesArray[WaveNode->WaveID].WaveSettings.EnemyTypes.Add(*enemyType);
											RefreshDisplayedDataAssets();
											return FReply::Handled();
										})
							]
						]
						+ SVerticalBox::Slot()
						[
							SNew(SComboBox<TSharedPtr<ESpawnOrder>>)
								.OptionsSource(&SpawnOrders)
								.OnGenerateWidget_Lambda([this](TSharedPtr<ESpawnOrder> Item)
									{
										return SNew(STextBlock).Text(EnumToText(*Item));
									})
								.OnSelectionChanged_Lambda(
									[this, WaveNode]
									(TSharedPtr<ESpawnOrder> NewValue, ESelectInfo::Type SelectInfo)
									{
										if (NewValue.IsValid())
										{
											UE_LOG(LogTemp, Warning, TEXT("Selected: %d"), static_cast<int32>(*NewValue));
											SelectedSpawnOrder = NewValue;
											DataWaveContainersArray[WaveNode->WaveContainerID]->WavesArray[WaveNode->WaveID].SpawnOrder = *NewValue.Get();
											WaveContainerNodes[WaveNode->WaveContainerID]->WavesArray[WaveNode->WaveID]->SpawnOrder = *NewValue.Get();
										}
									})
								.InitiallySelectedItem(SelectedSpawnOrder)
								[
									SNew(STextBlock)
										.Text_Lambda([this, WaveNode]()
											{
												return EnumToText(WaveContainerNodes[WaveNode->WaveContainerID]->WavesArray[WaveNode->WaveID]->SpawnOrder);
											})
								]
						]
					]
					+ SHorizontalBox::Slot().VAlign(EVerticalAlignment::VAlign_Center).HAlign(EHorizontalAlignment::HAlign_Right).Padding(0, 0, 15, 0)
					[
						SNew(SButton)
							.Text(FText::FromString(TEXT("-")))
							.ToolTipText(FText::FromString(TEXT("Remove Wave")))
							.OnClicked_Lambda([this, WaveNode]()
								{
									DataWaveContainersArray[WaveNode->WaveContainerID]->WavesArray.RemoveAt(WaveNode->WaveID);
									SaveWaveDataAsset(DataWaveContainersArray[WaveNode->WaveContainerID]);
									RefreshDisplayedDataAssets();
									return FReply::Handled();
								})
					]
				]
			]
		];
		break;
	}
	case ETreeNodeType::EnemyType:
	{
		TSharedPtr<FEnemyTypesNode> EnemyType = StaticCastSharedPtr<FEnemyTypesNode>(InItem);
		
		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable)
		[
			SNew(SBox).Padding(0, 5)
			[
				SNew(SBorder).Padding(5, 7)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						[
							SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight()
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
												.SelectedClass_Lambda([this, EnemyType]()
													{
														return WaveContainerNodes[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID]->EnemyTypes[EnemyType->EnemyTypesID]->EnemyClass.Get();
													})
												.AllowNone(true)
												.OnSetClass_Lambda([this, EnemyType](const UClass* SelectedClass) {
												this->OnClassSelected(SelectedClass, EnemyType->WaveContainerID, EnemyType->WaveID, EnemyType->EnemyTypesID);
													})
										]
								]
								+ SVerticalBox::Slot().AutoHeight()
								[
									SNew(SHorizontalBox) + SHorizontalBox::Slot()
										[
											SNew(STextBlock).Text(FText::FromString(TEXT("Use Enemy Cost")))
										]
										+ SHorizontalBox::Slot()
										[
											SNew(SCheckBox)
												.IsChecked_Lambda([this, EnemyType]()
													{
														return GetWaveEnemyUseEnemyCostState(EnemyType->WaveContainerID, EnemyType->WaveID, EnemyType->EnemyTypesID);
													})
												.OnCheckStateChanged_Lambda([this, EnemyType](ECheckBoxState NewState)
													{
														WaveContainerNodes[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID]->EnemyTypes[EnemyType->EnemyTypesID]->UseEnemyCost = (NewState == ECheckBoxState::Checked);
													})
										]
								]
								+ SVerticalBox::Slot()
								[
									SNew(SHorizontalBox) + SHorizontalBox::Slot()
										[
											SNew(STextBlock).Text(FText::FromString(TEXT("Min Enemies")))
										]
										+ SHorizontalBox::Slot()
										[
											SNew(SSpinBox<int32>)
												.OnValueChanged_Lambda([this, EnemyType](const int32 NewValue)
													{
														WaveContainerNodes[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID]->EnemyTypes[EnemyType->EnemyTypesID]->MinEnemies = NewValue;
													})
												.MinValue(0)
												.IsEnabled_Lambda([this, EnemyType]()
													{
														return GetWaveEnemyUseEnemyCostState(EnemyType->WaveContainerID, EnemyType->WaveID, EnemyType->EnemyTypesID) == ECheckBoxState::Unchecked;
													})
												.Value_Lambda([this, EnemyType]()
													{
														return WaveContainerNodes[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID]->EnemyTypes[EnemyType->EnemyTypesID]->MinEnemies;
													})
										]
								]
								+ SVerticalBox::Slot()
								[
									SNew(SHorizontalBox) + SHorizontalBox::Slot()
										[
											SNew(STextBlock).Text(FText::FromString(TEXT("Max Enemies")))
										]
										+ SHorizontalBox::Slot()
										[
											SNew(SSpinBox<int32>)
												.OnValueChanged_Lambda([this, EnemyType](const int32 NewValue)
													{
														WaveContainerNodes[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID]->EnemyTypes[EnemyType->EnemyTypesID]->MaxEnemies = NewValue;
													})
												.MinValue(0)
												.IsEnabled_Lambda([this, EnemyType]()
													{
														return GetWaveEnemyUseEnemyCostState(EnemyType->WaveContainerID, EnemyType->WaveID, EnemyType->EnemyTypesID) == ECheckBoxState::Unchecked;
													})
												.Value_Lambda([this, EnemyType]()
													{
														return WaveContainerNodes[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID]->EnemyTypes[EnemyType->EnemyTypesID]->MaxEnemies;
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
														WaveContainerNodes[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID]->EnemyTypes[EnemyType->EnemyTypesID]->Cost = NewValue;
													})
												.MinValue(0)
												.Value_Lambda([this, EnemyType]()
													{
														return WaveContainerNodes[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID]->EnemyTypes[EnemyType->EnemyTypesID]->Cost;
													})
												.IsEnabled_Lambda([this, EnemyType]()
													{
														return GetWaveEnemyUseEnemyCostState(EnemyType->WaveContainerID, EnemyType->WaveID, EnemyType->EnemyTypesID) == ECheckBoxState::Checked;
													})
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
												.OnGenerateWidget_Lambda([this](TSharedPtr<EEnemyTypes> Item)
													{
														return SNew(STextBlock).Text(EnumToText(*Item));
													})
												.OnSelectionChanged_Lambda(
													[this, EnemyType]
													(TSharedPtr<EEnemyTypes> NewValue, ESelectInfo::Type SelectInfo)
													{
														if (NewValue.IsValid())
														{
															UE_LOG(LogTemp, Warning, TEXT("Selected: %d"), static_cast<int32>(*NewValue));
															SelectedEnemyType = NewValue;
															DataWaveContainersArray[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID].WaveSettings.EnemyTypes[EnemyType->EnemyTypesID].EnemyType = *NewValue.Get();
															WaveContainerNodes[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID]->EnemyTypes[EnemyType->EnemyTypesID]->EnemyType = *NewValue.Get();
														}
													})
												.InitiallySelectedItem(SelectedEnemyType)
												[
													SNew(STextBlock)
														.Text_Lambda([this, EnemyType]()
															{
																return EnumToText(WaveContainerNodes[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID]->EnemyTypes[EnemyType->EnemyTypesID]->EnemyType);
															})
												]
										]
								]
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.Padding(15, 0)
						[
							SNew(SButton)
								.Text(FText::FromString(TEXT("-")))
								.ToolTipText(FText::FromString(TEXT("Delete EnemyType Entry")))
								.OnClicked_Lambda([this, EnemyType]()
									{
										DataWaveContainersArray[EnemyType->WaveContainerID]->WavesArray[EnemyType->WaveID].WaveSettings.EnemyTypes.RemoveAt(EnemyType->EnemyTypesID);
										SaveWaveDataAsset(DataWaveContainersArray[EnemyType->WaveContainerID]);
										RefreshDisplayedDataAssets();
										return FReply::Handled();
									})
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

ECheckBoxState SWaveToolWidgetMenu::GetWaveEnemyUseEnemyCostState(const int32 containerIndex, const int32 waveIndex, const int32 enemyTypeIndex) const
{
	return WaveContainerNodes[containerIndex]->WavesArray[waveIndex]->EnemyTypes[enemyTypeIndex]->UseEnemyCost == true ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

FReply SWaveToolWidgetMenu::OnWaveContainerSaveButtonClicked(TSharedPtr<FWaveContainerNode> containerNode)
{
	DeleteDataAsset(DataWaveContainersArray[containerNode->WaveContainerID]->GetName());
	FString path = WaveContainerPath + WaveContainerNodes[containerNode->WaveContainerID]->WaveContainerName;
	//create a new UPDataWaveAsset and copy all the values from the corresponding WaveContainerNode
	UPDataWaveContainer* NewDataAsset = NewObject<UPDataWaveContainer>();	// since UPDataWaveContainer is a UObject
	NewDataAsset->Rename(*WaveContainerNodes[containerNode->WaveContainerID]->WaveContainerName);
	//modify the values of the container
	//save the container
	NewDataAsset->DescriptionOfContainer = WaveContainerNodes[containerNode->WaveContainerID]->ContainerDescription;
	int32 waveArrayIndex = 0;
	for (auto& wave : WaveContainerNodes[containerNode->WaveContainerID]->WavesArray)
	{
		FWaveOrder Wave;
		Wave.WaveSettings.Description = wave->Description;
		Wave.WaveSettings.TotalPoints = wave->TotalPoints;
		Wave.SpawnOrder = wave->SpawnOrder;
		for (auto& enemyType : wave->EnemyTypes)
		{
			FInternalDumbEnemyType EnemyType;
			EnemyType.EnemyClass = enemyType->EnemyClass;
			EnemyType.bUseEnemyCost = enemyType->UseEnemyCost;
			EnemyType.MinEnemies = enemyType->MinEnemies;
			EnemyType.MaxEnemies = enemyType->MaxEnemies;
			EnemyType.Cost = enemyType->Cost;
			EnemyType.EnemyType = enemyType->EnemyType;
			Wave.WaveSettings.EnemyTypes.Add(EnemyType);
		}
		NewDataAsset->WavesArray.Add(Wave);
		waveArrayIndex++;
	}
	DataWaveContainersArray.Insert(NewDataAsset, containerNode->WaveContainerID);
	SaveWaveDataAsset(NewDataAsset);
	RefreshDisplayedDataAssets();
	return FReply::Handled();
}

void SWaveToolWidgetMenu::FindOrCreateManager()
{
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return;

	for (TActorIterator<APWaveManager> ItActor(World); ItActor; ++ItActor)
	{
		WaveManager = *ItActor;
		UE_LOG(LogTemp, Log, TEXT("Found Wave Manager"));
		break;
	}

	if (!WaveManager)
	{
		UE_LOG(LogTemp, Log, TEXT("Creating Wave Manager"));
		WaveManager = NewObject<APWaveManager>(World);
	}
}

// ENUM DROP DOWN MENU //
FText SWaveToolWidgetMenu::EnumToText(EEnemyTypes EnumValue) const
{
	switch (EnumValue)
	{
	case EEnemyTypes::Melee:
		return FText::FromString(TEXT("Melee"));
		break;
	case EEnemyTypes::Ranged:
		return FText::FromString(TEXT("Ranged"));
		break;
	case EEnemyTypes::Tank:
		return FText::FromString(TEXT("Tank"));
		break;
	default:
		return FText::FromString(TEXT("Unknown"));
		break;
	}
}

// Spawn Order Drop Down Menu //
FText SWaveToolWidgetMenu::EnumToText(ESpawnOrder EnumValue) const
{
	switch (EnumValue)
	{
	case ESpawnOrder::Order1:
		return FText::FromString(TEXT("Order1"));
		break;
	case ESpawnOrder::Order2:
		return FText::FromString(TEXT("Order2"));
		break;
	case ESpawnOrder::Order3:
		return FText::FromString(TEXT("Order3"));
		break;
	case ESpawnOrder::Order4:
		return FText::FromString(TEXT("Order4"));
		break;
	case ESpawnOrder::Order5:
		return FText::FromString(TEXT("Order5"));
		break;
	case ESpawnOrder::Order6:
		return FText::FromString(TEXT("Order6"));
		break;
	case ESpawnOrder::Order7:
		return FText::FromString(TEXT("Order7"));
		break;
	case ESpawnOrder::Order8:
		return FText::FromString(TEXT("Order8"));
		break;
	case ESpawnOrder::Order9:
		return FText::FromString(TEXT("Order9"));
		break;
	case ESpawnOrder::Order10:
		return FText::FromString(TEXT("Order10"));
		break;
	default:
		return FText::FromString(TEXT("Unknown"));
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
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected class is not a valid AActor subclass."));
	}
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

bool SWaveToolWidgetMenu::DeleteDataAsset(FString assetName)
{
	FString assetPathToDelete = "";
	assetPathToDelete.Append(WaveContainerPath);
	assetPathToDelete.Append(assetName);
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

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
/*
* longpackagename warning fix
*/