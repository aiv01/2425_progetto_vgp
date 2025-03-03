// Fill out your copyright notice in the Description page of Project Settings.


#include "SWaveToolWidgetMenu.h"
#include "SlateOptMacros.h"
#include "PWaveManager.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SWaveToolWidgetMenu::Construct(const FArguments& InArgs)
{
	DataWaveContainer = InArgs._DataWaveContainer;
	//The issue is that your widget isn't assigning its UI tree to its root slot. 
	// In Slate (when subclassing SCompoundWidget), you must wrap your UI in a ChildSlot
	ChildSlot
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock).Text(FText::FromString("Data Asset Properties"))
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SAssignNew(PropertyList, SVerticalBox)
				]
			+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SButton)
						.Text(FText::FromString("SetWaveData"))
						.OnClicked(this, &SWaveToolWidgetMenu::OnSetWaveDataButtonClicked)
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
	if (!DataWaveContainer || !PropertyList.IsValid())
	{
		return;
	}

	int32 waveContainerID = DataWaveContainer->WaveContainerID;
	FString descriptionOfContainer = DataWaveContainer->DescriptionOfContainer;
	TArray<FWaveOrder> wavesArray = DataWaveContainer->WavesArray;

	/*
	for (TFieldIterator<FProperty> It(AssetClass); It; ++It)	//make iterator of AssetClass
	{
		FProperty* Property = *It;

		if (Property)
		{
			FString PropertyName = Property->GetName();
			FString PropertyValue;

			//cast to different types, if so, take the value
			if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
			{
				PropertyValue = StrProp->GetPropertyValue_InContainer(DataAsset);
			}
			else if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
			{
				PropertyValue = FString::Printf(TEXT("%d"), IntProp->GetPropertyValue_InContainer(DataAsset));
			}
			else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
			{
				PropertyValue = FString::Printf(TEXT("%.2f"), FloatProp->GetPropertyValue_InContainer(DataAsset));
			}
			else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
			{
				TArray<> ArrayPropertyValue =
				UE_LOG(LogTemp, Log, TEXT("Found Array"));
			}

			PropertyList->AddSlot()
				.AutoHeight()
				[
					SNew(STextBlock)
						.Text(FText::FromString(FString::Printf(TEXT("%s: %s"), *PropertyName, *PropertyValue)))
				];
		}
	}
	*/
	PropertyList->AddSlot().AutoHeight()
		[
			SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
				[
					SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Wave Container ID: %d\n"), waveContainerID)))
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Description: %s\n"), *descriptionOfContainer)))
				]
		];
	/*PropertyList->AddSlot().AutoHeight()
	[
		SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Wave Container ID: %d"), waveContainerID)))
	];
	PropertyList->AddSlot().AutoHeight()
	[
		SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Description: %s"), *descriptionOfContainer)))
	];*/
	for (int32 i = 0; i < wavesArray.Num(); i++)
	{
		PropertyList->AddSlot().AutoHeight()
			[
				SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight()
					[
						SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
							[
								SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Wave ID: %d\n"), wavesArray[i].WaveSettings.WaveID)))
							]
					] + SVerticalBox::Slot().AutoHeight()
							[
								SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
									[
										SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Description: %s\n"), *wavesArray[i].WaveSettings.Description)))
									]
							] + SVerticalBox::Slot().AutoHeight()
									[
										SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
											[
												SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Total Points: %d\n"), wavesArray[i].WaveSettings.TotalPoints)))
											]
									] + SVerticalBox::Slot().AutoHeight()
											[
												SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
													[
														SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Enemy Types"))))
													]
											]
			];
		for (int32 j = 0; j < wavesArray[i].WaveSettings.EnemyTypes.Num(); j++)
		{
			PropertyList->AddSlot()
				[
					SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()
						[
							SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Enemy Class: %s"),
								wavesArray[i].WaveSettings.EnemyTypes[j].EnemyClass != NULL ? *wavesArray[i].WaveSettings.EnemyTypes[j].EnemyClass->GetName() : TEXT("NULL"))))
						]
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Cost: %d"), wavesArray[i].WaveSettings.EnemyTypes[j].Cost)))
						]
						+ SHorizontalBox::Slot().AutoWidth()
						[
							SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("Enemy Type: %d"), wavesArray[i].WaveSettings.EnemyTypes[j].EnemyType)))
						]
				];
		};
	};
}
FReply SWaveToolWidgetMenu::OnSetWaveDataButtonClicked()
{
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Warning, TEXT("Set Wave Data"));
#endif
	APWaveManager::Instance->SetWaveData(DataWaveContainer, false);
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
