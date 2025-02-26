// Copyright Epic Games, Inc. All Rights Reserved.

#include "PNetworking.h"
#include "OnlineSubsystem.h"

IOnlineSubsystem* FPNetworkingModule::OnlineSubsystemReference = nullptr;

#define LOCTEXT_NAMESPACE "FPNetworkingModule"

void FPNetworkingModule::StartupModule()
{
	OnlineSubsystemReference = IOnlineSubsystem::Get();
}

void FPNetworkingModule::ShutdownModule()
{
	delete OnlineSubsystemReference;
}

IOnlineSubsystem* FPNetworkingModule::GetOnlineSubsystemReference()
{
	if (!OnlineSubsystemReference)
	{
#pragma region LOGS
		UE_LOG(LogTemp, Error, TEXT("Online Subsystem Not Initialized!"));
#pragma endregion LOGS
	}

	return OnlineSubsystemReference;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPNetworkingModule, PNetworking)