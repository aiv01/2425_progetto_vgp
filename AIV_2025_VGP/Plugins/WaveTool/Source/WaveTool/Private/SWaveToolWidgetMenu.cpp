// Fill out your copyright notice in the Description page of Project Settings.


#include "SWaveToolWidgetMenu.h"
#include "SlateOptMacros.h"
#include "PWaveManager.h"
#include <FileHelpers.h>
#include "EditorAssetLibrary.h"

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
				[
					SNew(STextBlock).Text(FText::FromString("Data Asset Properties"))
				]
					+ SVerticalBox::Slot()
				[
					SAssignNew(PropertyList, SVerticalBox)
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
						.Text(FText::FromString("SetWaveData"))
						.OnClicked(this, &SWaveToolWidgetMenu::OnSetWaveDataButtonClicked)
				] + SVerticalBox::Slot().AutoHeight()
				[
					SNew(SButton)
                        .HAlign(HAlign_Center)
						.Text(FText::FromString("Save WaveDataAsset"))
						.OnClicked(this, &SWaveToolWidgetMenu::SaveWaveDataAsset)
				]
			]
		]
	];
	GenerateDataTableWidget();
}

FReply SWaveToolWidgetMenu::OnTestButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Button clicked, and the checkbox is %s"), (bIsTestBoxChecked ? TEXT("checked") : TEXT("unchecked")));
	return FReply::Handled();
}

void SWaveToolWidgetMenu::OnTestCheckBoxChanged(ECheckBoxState NewState)
{
	bIsTestBoxChecked = NewState == ECheckBoxState::Checked ? true : false;
}

ECheckBoxState SWaveToolWidgetMenu::IsTestCheckBoxChecked() const
{
	return bIsTestBoxChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
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
				SNew(STextBlock).Text(FText::FromString("No DataWaveContainers found"))
			];
		return;
	}

	// Store the data
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
			for (int32 j = 0; j < DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes.Num(); j++)
			{
				TSharedPtr<FEnemyTypesNode> enemyTypesNode = MakeShared<FEnemyTypesNode>();
				enemyTypesNode->Cost = DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].Cost;
				enemyTypesNode->EnemyClass = &DataWaveContainer->WavesArray[i].WaveSettings.EnemyTypes[j].EnemyClass;
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
		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable)
			[
				SNew(STextBlock).Text(FText::FromString(ContainerNode->WaveContainerName))
			];
		break;
	}
	case ETreeNodeType::WavesArray:
	{
		TSharedPtr<FWavesArrayNode> WaveNode = StaticCastSharedPtr<FWavesArrayNode>(InItem);
		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable)
			[
				SNew(SVerticalBox) + 
					SVerticalBox::Slot().AutoHeight()
					[
						SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Wave ID: %d"), WaveNode->WaveID)))
					] + SVerticalBox::Slot().AutoHeight()
					[
						SNew(STextBlock).Text(FText::FromString(WaveNode->Description))
					] +
					SVerticalBox::Slot().AutoHeight()
					[
						SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Total Points: %d"), WaveNode->TotalPoints)))
					]
			];
		break;
	}
	case ETreeNodeType::EnemyType:
	{
		TSharedPtr<FEnemyTypesNode> EnemyNode = StaticCastSharedPtr<FEnemyTypesNode>(InItem);
		FString EnemyClassName = EnemyNode->EnemyClass->Get()->GetName();
		FString EnemyTypeName = *UEnum::GetDisplayValueAsText(EnemyNode->EnemyType).ToString();
		return SNew(STableRow<TSharedPtr<FBaseTreeNode>>, OwnerTable)
			[
				SNew(SVerticalBox) +
				SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Enemy Class: %s"), *EnemyClassName)))
				] + SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Enemy Cost: %d"), EnemyNode->Cost)))
				] + SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Enemy Type: %s"), *EnemyTypeName)))
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

	/*
	// Check which type of node we're dealing with.
	if (TSharedPtr<FWaveContainerNode> ContainerNode = CastSharedPtr<FWaveContainerNode>(InItem))
	{
		OutChildren = ContainerNode->Children;
	}
	else if (TSharedPtr<FWavesArrayNode> WaveNode = StaticCastSharedPtr<FWavesArrayNode>(InItem))
	{
		OutChildren = WaveNode->Children;
	}
	else if (TSharedPtr<FEnemyTypesNode> EnemyNode = StaticCastSharedPtr<FEnemyTypesNode>(InItem))
	{
		// FEnemyTypesNode has no children.
		OutChildren.Empty();
	}
	
	*/
}


FReply SWaveToolWidgetMenu::OnSetWaveDataButtonClicked()
{
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Warning, TEXT("Set Wave Data"));
#endif
	//APWaveManager::Instance->SetWaveData(DataWaveContainer, false);
	return FReply::Handled();
}

FReply SWaveToolWidgetMenu::SaveWaveDataAsset()
{
	FString PackageName = TEXT("/Game/Custom/WaveSystem/WaveContainers/NewDataAsset");
	FString AssetName = TEXT("NewDataAsset");

	PackageName = UPackageTools::SanitizePackageName(PackageName);
	//TODO: add check if exists already an asset with the same name

	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create package"));
		return FReply::Unhandled();
	}

	UPDataWaveContainer* NewDataAsset = NewObject<UPDataWaveContainer>(Package, UPDataWaveContainer::StaticClass(), *AssetName, RF_Public | RF_Standalone);
	if (!NewDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create asset"));
		return FReply::Unhandled();
	}
	NewDataAsset->MarkPackageDirty();

	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	bool bSaved = UPackage::SavePackage(Package, NewDataAsset, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName);

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

*/