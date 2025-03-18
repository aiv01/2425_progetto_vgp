// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "WaveToolStyle.h"

class FWaveToolCommands : public TCommands<FWaveToolCommands>
{
public:

	FWaveToolCommands()
		: TCommands<FWaveToolCommands>(TEXT("WaveTool"), NSLOCTEXT("Contexts", "WaveTool", "WaveTool Plugin"), NAME_None, FWaveToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};