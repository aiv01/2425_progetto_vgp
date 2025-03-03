// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "PDataWaveContainer.h"

class WAVETOOL_API SWaveToolWidgetMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWaveToolWidgetMenu)
	{}
		SLATE_ARGUMENT(UPDataWaveContainer*, DataWaveContainer)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	FReply OnTestButtonClicked();
	void OnTestCheckBoxChanged(ECheckBoxState NewState);
	ECheckBoxState IsTestCheckBoxChecked() const;

private:
	bool bIsTestBoxChecked;
	UPDataWaveContainer* DataWaveContainer;
	TSharedPtr<SVerticalBox> PropertyList;
	void GenerateDataTableWidget();
	FReply OnSetWaveDataButtonClicked();
};
