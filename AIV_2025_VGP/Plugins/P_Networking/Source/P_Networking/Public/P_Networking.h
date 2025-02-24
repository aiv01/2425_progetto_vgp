// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FP_NetworkingModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	class IOnlineSubsystem* onlineSubsystemReference = nullptr;
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	IOnlineSubsystem* GetOnlineSubsystemReference() const;
};
