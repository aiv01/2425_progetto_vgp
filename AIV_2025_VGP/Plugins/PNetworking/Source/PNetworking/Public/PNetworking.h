// Copyright Epic Games, Inc. All Rights Reserved.

// • Manuel Solano
// • Alessandro Caccamo
// • Claudio Dallai

#pragma once

#include "SteamAPICallbackManager.h"
#include "Modules/ModuleManager.h"

// Hardcoded Session Name. It needs to be the same on Client and Server even when inviting.
#define SESSION_NAME "AIV_VGP_Server"

// Forward declarations.
class IOnlineSubsystem;
class IOnlineSession;

// Custom Log category.
DECLARE_LOG_CATEGORY_EXTERN(LogSteamNetworkingPlugin, Warning, All)

// Identify current session state. 
enum ELocalSessionState : uint8
{
	SESSION_PENDING, // Computing a session (used for creating or other async funcs).
	SESSION_DESTROYING, // Destroying a session (specific for destroy async funcs).
	SESSION_VALID, // Session is present and working.
	SESSION_INVALID // Session is not present or socket is invalid.
};

class PNETWORKING_API FPNetworkingModule : public IModuleInterface
{
public:

	// Module base methods.
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#pragma region OnlineManagement

	// Check if everything is correct in order to use OSS/steam_api.
	static bool IsOnlineAvailable(const FString Message = TEXT("Online checked called"));

	// Initialize OSS and pointers.
	static void InternalStartupModule();

	// Getters.
	static IOnlineSubsystem* GetOnlineSubsystemPointer();
	static TSharedPtr<IOnlineSession, ESPMode::ThreadSafe> GetOnlineSessionPointer();
	static TSharedPtr<SteamAPICallbackManager> GetSteamAPIManager();
	static FName GetSessionName();
	static ELocalSessionState GetLocalSessionCurrentState();

	// Setters.
	static void SetLocalSessionCurrentState(const ELocalSessionState NewSessionState);

#pragma endregion

private:

#pragma region OnlineManagement

	// Hardcoded name of the session. Declared in .cpp file.
	static FName SessionName;

	// Current state of the session on this user-side.
	static ELocalSessionState LocalSessionCurrentState;

	// OSS pointers.
	static class IOnlineSubsystem* OnlineSubsystemPtr;
	static class TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionPtr;

	// SteamAPI callbacks manager class.
	static TSharedPtr<SteamAPICallbackManager> SteamApiManagerPtr;

#pragma endregion

};
