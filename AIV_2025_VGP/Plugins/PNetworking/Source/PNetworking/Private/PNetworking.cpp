// Copyright Epic Games, Inc. All Rights Reserved.

// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#include "PNetworking.h"
#include "OnlineSubsystem.h"

#define LOCTEXT_NAMESPACE "FPNetworkingModule"

// Initialization of static variables.
IOnlineSubsystem* FPNetworkingModule::OnlineSubsystemReference = nullptr;
IOnlineSessionPtr FPNetworkingModule::OnlineSessionReference = nullptr;
TSharedPtr<SteamAPICallbackManager> FPNetworkingModule::SteamApiManagerPtr = nullptr;
bool FPNetworkingModule::bIsComputingNewSession = false;
FName FPNetworkingModule::SessionName = TEXT("AIV_VGP3_Server"); // Name used for the multiplayer steam session.

// Define custom Log category.
DEFINE_LOG_CATEGORY(LogSteamNetworkingPlugin)

#pragma region ModuleManagement 

void FPNetworkingModule::StartupModule()
{
	// Get OSS.
	OnlineSubsystemReference = IOnlineSubsystem::Get();
	if (OnlineSubsystemReference)
	{
		// Get OSS Session interface.
		OnlineSessionReference = OnlineSubsystemReference->GetSessionInterface();
	}

	SteamApiManagerPtr = MakeShared<SteamAPICallbackManager>();
}

void FPNetworkingModule::ShutdownModule()
{
	// Cleanup pointers, refs...
	OnlineSessionReference.Reset();
	OnlineSubsystemReference = nullptr;
	SteamApiManagerPtr.Reset();
}

#pragma endregion

#pragma region OnlineManagement

// Check if Steam and Standalone mode are available and working.
bool FPNetworkingModule::IsOnlineAvailable()
{
	// The game must be run in standalone!
	if (GIsEditor)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("ERROR: Online Subsystem doesn't work because the game is loaded as PIE!"));
		return false;
	}

	// The Steam client must be opened!
	if (!SteamAPI_IsSteamRunning())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("ERROR: Online Subsystem doesn't work because Steam isn't opened on the client!"));
		return false;
	}

	// OSS not valid!
	if (!OnlineSubsystemReference)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("ERROR: Online Subsystem isn't initialized!"));
		return false;
	}

	// SharedPtr of Session interface in OSS not valid!
	if (!OnlineSessionReference.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("ERROR: Online Session Interface isn't available!"));
		return false;
	}

	return true;
}

// Get OSS pointer.
IOnlineSubsystem* FPNetworkingModule::GetOnlineSubsystemReference()
{
	if (!IsOnlineAvailable())
	{
		return nullptr;
	}

	return OnlineSubsystemReference;
}

// Get OSS SessionInterface SharedPtr.
IOnlineSessionPtr FPNetworkingModule::GetOnlineSessionReference()
{
	if (!IsOnlineAvailable())
	{
		return nullptr;
	}

	return OnlineSessionReference;
}

// Getter of hardcoded name of the session.
FName FPNetworkingModule::GetSessionName()
{
	return SessionName;
}

TSharedPtr<SteamAPICallbackManager> FPNetworkingModule::GetSteamAPIManager()
{
	if (!IsOnlineAvailable())
	{
		return nullptr;
	}

	return SteamApiManagerPtr;
}

#pragma endregion

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPNetworkingModule, PNetworking)