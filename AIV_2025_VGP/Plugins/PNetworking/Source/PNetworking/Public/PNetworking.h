// Copyright Epic Games, Inc. All Rights Reserved.

// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#pragma once

#include "Modules/ModuleManager.h"

class PNETWORKING_API FPNetworkingModule : public IModuleInterface
{
public:
	static bool bIsComputingNewSession;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	static class IOnlineSubsystem* GetOnlineSubsystemReference();
	static class TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> GetOnlineSessionReference();
	static bool IsOnlineAvailable();
	static FName GetSessionName();

private:
	static FName SessionName;

	static class IOnlineSubsystem* OnlineSubsystemReference;
	static class TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionReference;
};
