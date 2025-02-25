// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class P_NETWORKING_API FP_NetworkingModule : public IModuleInterface
{
	/** IModuleInterface implementation */

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	static class IOnlineSubsystem* GetOnlineSubsystemReference();

private:
	static class IOnlineSubsystem* onlineSubsystemReference;
};
