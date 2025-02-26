// Copyright Epic Games, Inc. All Rights Reserved.

#include "PNetworkingBPLibrary.h"
#include "OnlineSubsystem.h"
#include "PNetworking.h"

UPNetworkingBPLibrary::UPNetworkingBPLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

FString UPNetworkingBPLibrary::GetAppID()
{
	// Convert this to preprocessor macro (maybe).
	IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

	if (!OnlineSubsystemReference)
	{
		return FString("ERROR!");
	}

	return OnlineSubsystemReference->GetAppId();
}

FString UPNetworkingBPLibrary::GetAccountName(const uint32 UserID)
{
	IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

	if (!OnlineSubsystemReference)
	{
		return FString("ERROR!");
	}

	IOnlineIdentityPtr OnlineIdentityPtr = OnlineSubsystemReference->GetIdentityInterface();
	if (!OnlineIdentityPtr.IsValid())
	{
		return FString("ERROR!");
	}

	// Beccare un metodo che resituisce la friendlist e stamparla.

	return FString(); // Fix
}
