// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaveToolCommands.h"

#define LOCTEXT_NAMESPACE "FWaveToolModule"

void FWaveToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "WaveTool", "Bring up WaveTool window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
