// Copyright Epic Games, Inc. All Rights Reserved.

// • Manuel Solano
// • Alessandro Caccamo
// • Claudio Dallai

#include "PNetworking.h"
#include "OnlineSubsystem.h"

#define LOCTEXT_NAMESPACE "FPNetworkingModule"

// Initialization of static variables.
IOnlineSubsystem* FPNetworkingModule::OnlineSubsystemPtr = nullptr;
IOnlineSessionPtr FPNetworkingModule::OnlineSessionPtr = nullptr;
TSharedPtr<SteamAPICallbackManager> FPNetworkingModule::SteamApiManagerPtr = nullptr;
ELocalSessionState FPNetworkingModule::LocalSessionCurrentState = ELocalSessionState::SESSION_INVALID;
FName FPNetworkingModule::SessionName = TEXT(SESSION_NAME); // Name used for the multiplayer steam session.

// Define custom Log category.
DEFINE_LOG_CATEGORY(LogSteamNetworkingPlugin)

#pragma region ModuleManagement 

void FPNetworkingModule::StartupModule()
{
	InternalStartupModule();
}

void FPNetworkingModule::ShutdownModule()
{
	// Cleanup pointers, refs...
	OnlineSubsystemPtr = nullptr;
	OnlineSessionPtr.Reset();
	SteamApiManagerPtr.Reset();
}

#pragma endregion

#pragma region OnlineManagement

// Check if Steam and Standalone mode are available and working.
bool FPNetworkingModule::IsOnlineAvailable(const FString Message)
{
	UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("%s"), *Message);

#if WITH_EDITOR
	// The game must be run in standalone!
	if (GIsEditor)
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("ERROR: Online Subsystem doesn't work because the game is loaded as PIE!"));
		return false;
	}
#endif

	// The Steam client must be opened!
	if (!SteamAPI_IsSteamRunning())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("ERROR: Online Subsystem doesn't work because Steam isn't opened on the client!"));
		return false;
	}

	// OSS not valid!
	if (!OnlineSubsystemPtr)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("ERROR: Online Subsystem isn't initialized!"));
		return false;
	}

	// SharedPtr of Session interface in OSS not valid!
	if (!OnlineSessionPtr.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("ERROR: Online Session Interface isn't available!"));
		return false;
	}

	// SteamApiManagerPtr is not valid!
	if (!SteamApiManagerPtr.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("ERROR: SteamAPIManager isn't available!"));
		return false;
	}

	return true;
}

// Called to initialize this module inside StartupModule method.
void FPNetworkingModule::InternalStartupModule()
{
	// Get OSS.
	OnlineSubsystemPtr = IOnlineSubsystem::Get();
	if (OnlineSubsystemPtr)
	{
		// Get OSS Session interface.
		OnlineSessionPtr = OnlineSubsystemPtr->GetSessionInterface();
	}

	SteamApiManagerPtr = MakeShared<SteamAPICallbackManager>();
}

// Get OSS pointer.
IOnlineSubsystem* FPNetworkingModule::GetOnlineSubsystemPointer()
{
	if (!IsOnlineAvailable(TEXT("IsOnlineAvailable: GetOnlineSubsystemPointer Called it")))
	{
		return nullptr;
	}

	return OnlineSubsystemPtr;
}

// Get OSS SessionInterface SharedPtr.
IOnlineSessionPtr FPNetworkingModule::GetOnlineSessionPointer()
{
	if (!IsOnlineAvailable(TEXT("IsOnlineAvailable: GetOnlineSessionPointer Called it")))
	{
		return nullptr;
	}

	return OnlineSessionPtr;
}

// Get SteamAPIManager SharedPtr. This uses steam folder files to implement steamworks sdk.
TSharedPtr<SteamAPICallbackManager> FPNetworkingModule::GetSteamAPIManager()
{
	if (!IsOnlineAvailable(TEXT("IsOnlineAvailable: GetSteamAPIManager Called it")))
	{
		return nullptr;
	}

	return SteamApiManagerPtr;
}

// Getter of hardcoded name of the session.
FName FPNetworkingModule::GetSessionName()
{
	return SessionName;
}

// Get current local session state.
ELocalSessionState FPNetworkingModule::GetLocalSessionCurrentState()
{
	return LocalSessionCurrentState;
}

// Set current local session state.
void FPNetworkingModule::SetLocalSessionCurrentState(const ELocalSessionState NewSessionState)
{
	if (LocalSessionCurrentState != NewSessionState)
	{
		LocalSessionCurrentState = NewSessionState;
	}
}

#pragma endregion

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPNetworkingModule, PNetworking)