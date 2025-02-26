// Copyright Epic Games, Inc. All Rights Reserved.

// © Manuel Solano
// © Alessandro Caccamo
// © Claudio Dallai

#include "PNetworkingBPLibrary.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "PNetworking.h"

UPNetworkingBPLibrary::UPNetworkingBPLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool UPNetworkingBPLibrary::GetAppID(FString& AppID)
{
	IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

	if (!OnlineSubsystemReference)
	{
		return false;
	}

	AppID = OnlineSubsystemReference->GetAppId();

	return true;
}

bool UPNetworkingBPLibrary::GetAccountName(FString& AccountName, const int32 UserID)
{
	IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

	if (!OnlineSubsystemReference)
	{
		return false;
	}

	IOnlineIdentityPtr OnlineIdentityPtr = OnlineSubsystemReference->GetIdentityInterface();
	if (!OnlineIdentityPtr.IsValid())
	{
		return false;
	}

	FUniqueNetIdPtr UniqueNetId = OnlineIdentityPtr->GetUniquePlayerId(0); // Local Player.

	if (!UniqueNetId.IsValid())
	{
		return false;
	}

	AccountName = OnlineIdentityPtr->GetPlayerNickname(*UniqueNetId);

	return true;
}

bool UPNetworkingBPLibrary::GetFriendsList(TArray<FString>& FriendsListNames, const int32 LocalUserNum)
{
	IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

	if (!OnlineSubsystemReference)
	{
		return false;
	}

	IOnlineFriendsPtr FriendsInterface = OnlineSubsystemReference->GetFriendsInterface();
	if (!FriendsInterface.IsValid())
	{
		return false;
	}

	// All friends into friendslist.
	TArray<TSharedRef<FOnlineFriend>> FriendsList;
	FriendsInterface->GetFriendsList(LocalUserNum, EFriendsLists::ToString(EFriendsLists::Default), FriendsList); // NOT WORKING

#pragma region LOGS
	UE_LOG(LogTemp, Warning, TEXT("Friends List Size: %d"), FriendsList.Num());
#pragma endregion LOGS

	// In order to make one syscall.
	FriendsListNames.Reserve(FriendsList.Num());

	for (const TSharedRef<FOnlineFriend>& Friend : FriendsList)
	{
		FriendsListNames.Add(Friend->GetDisplayName());
	}

	return true;
}