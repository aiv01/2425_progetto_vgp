// Copyright Epic Games, Inc. All Rights Reserved.

// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#pragma once

#include "SteamAPICallbackManager.h"
#include "Modules/ModuleManager.h"

// Custom Log category.
DECLARE_LOG_CATEGORY_EXTERN(LogSteamNetworkingPlugin, Warning, All)

class PNETWORKING_API FPNetworkingModule : public IModuleInterface
{
public:

	// Module base methods.
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#pragma region OnlineManagement

	// Session creation is currently computing. Need to be set/checked as user prefer, it is just a flag.
	static bool bIsComputingNewSession;

	// Check if everything is correct in order to use OSS/steam_api.
	static bool IsOnlineAvailable();

	// Getters.
	static class IOnlineSubsystem* GetOnlineSubsystemReference();
	static class TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> GetOnlineSessionReference();
	static FName GetSessionName();
	static TSharedPtr<SteamAPICallbackManager> GetSteamAPIManager();

#pragma endregion

private:

#pragma region OnlineManagement

	// Hardcoded name of the session. Declared in .cpp file.
	static FName SessionName;

	// OSS references.
	static class IOnlineSubsystem* OnlineSubsystemReference;
	static class TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionReference;

	// SteamAPI callbacks manager class.
	static TSharedPtr<SteamAPICallbackManager> SteamApiManagerPtr;

#pragma endregion
};
