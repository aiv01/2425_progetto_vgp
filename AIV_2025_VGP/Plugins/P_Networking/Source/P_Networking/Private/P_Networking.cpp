// Copyright Epic Games, Inc. All Rights Reserved.

#include "P_Networking.h"
#include "OnlineSubsystem.h"

#define LOCTEXT_NAMESPACE "FP_NetworkingModule"

void FP_NetworkingModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("MyOnlineSubsystemPlugin initializing OnlineSubsystem..."));

    onlineSubsystemReference = IOnlineSubsystem::Get();
    if (onlineSubsystemReference)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnlineSubsystem initialized!!!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnlineSubsystem Error!!!!"));
    }
}

void FP_NetworkingModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

IOnlineSubsystem* FP_NetworkingModule::GetOnlineSubsystemReference() const
{
    return onlineSubsystemReference;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FP_NetworkingModule, P_Networking)