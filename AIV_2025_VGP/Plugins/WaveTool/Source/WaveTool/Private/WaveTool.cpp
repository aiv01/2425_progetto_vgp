// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaveTool.h"
#include "WaveToolStyle.h"
#include "WaveToolCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "PDataWaveContainer.h"
#include "SWaveToolWidgetMenu.h"

static const FName WaveToolTabName("WaveTool");

#define LOCTEXT_NAMESPACE "FWaveToolModule"

void FWaveToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FWaveToolStyle::Initialize();
	FWaveToolStyle::ReloadTextures();

	FWaveToolCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FWaveToolCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FWaveToolModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWaveToolModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(WaveToolTabName, FOnSpawnTab::CreateRaw(this, &FWaveToolModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FWaveToolTabTitle", "WaveTool"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FWaveToolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FWaveToolStyle::Shutdown();

	FWaveToolCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(WaveToolTabName);
}

TSharedRef<SDockTab> FWaveToolModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SWaveToolWidgetMenu)
	];
}

void FWaveToolModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(WaveToolTabName);
}

void FWaveToolModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FWaveToolCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FWaveToolCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWaveToolModule, WaveTool)

/*
	data asset container viewer
	window to create custom enemies (textfield
	thing that shows all available enemies, and with button(s) make it create an
	enemy, or minus button to destroy an enemy

	editor time (asset waves)
	runtime (wave data asset: drop down menu)

	make it so certain values can be enabled or disabled via flag -> for cost in
	wave data asset, you can disable the cost to input a certain amount of enemies
*/