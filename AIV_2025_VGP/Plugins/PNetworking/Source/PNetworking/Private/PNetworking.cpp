// Copyright Epic Games, Inc. All Rights Reserved.

// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#include "PNetworking.h"
#include "steam/steam_api.h"
#include "OnlineSubsystem.h"

IOnlineSubsystem* FPNetworkingModule::OnlineSubsystemReference = nullptr;

#define LOCTEXT_NAMESPACE "FPNetworkingModule"

void FPNetworkingModule::StartupModule()
{
	OnlineSubsystemReference = IOnlineSubsystem::Get();
}

void FPNetworkingModule::ShutdownModule()
{

}

IOnlineSubsystem* FPNetworkingModule::GetOnlineSubsystemReference()
{
	if (!IsOnlineAvailable())
	{
		return nullptr;
	}
	
	return OnlineSubsystemReference;
}

bool FPNetworkingModule::IsOnlineAvailable()
{
	// The game must be run in standalone!
	if (GIsEditor)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Online Subsystem doesn't work because the game is loaded as PIE!"));
		return false;
	}

	// The Steam client must be opened!
	if (!SteamAPI_IsSteamRunning())
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Online Subsystem doesn't work because Steam isn't opened on the client!"));
		return false;
	}

	if (!OnlineSubsystemReference)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Online Subsystem isn't initialized!"));
		return false;
	}

	return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPNetworkingModule, PNetworking)