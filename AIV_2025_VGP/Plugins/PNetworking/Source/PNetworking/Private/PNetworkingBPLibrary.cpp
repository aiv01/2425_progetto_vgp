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

	FUniqueNetIdPtr UniqueNetId = OnlineIdentityPtr->GetUniquePlayerId(UserID); // Local Player.

	if (!UniqueNetId.IsValid())
	{
		return false;
	}

	AccountName = OnlineIdentityPtr->GetPlayerNickname(*UniqueNetId);

	return true;
}

bool UPNetworkingBPLibrary::GetFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum)
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

	// Read the friends list before get all the names.
	const bool bGetFriendsSuccessful = FriendsInterface->ReadFriendsList(LocalUserNum, EFriendsLists::ToString(EFriendsLists::Default), FOnReadFriendsListComplete::CreateLambda([Callback](int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr) mutable
		{
			TArray<FString> FriendsListNames;

			if (!bWasSuccessful)
			{
				return;
			}

			IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

			if (!OnlineSubsystemReference)
			{
				return;
			}

			IOnlineFriendsPtr OnlineFriendsPtr = OnlineSubsystemReference->GetFriendsInterface();
			if (!OnlineFriendsPtr.IsValid())
			{
				return;
			}

			TArray<TSharedRef<FOnlineFriend>> FriendsList;
			OnlineFriendsPtr->GetFriendsList(LocalUserNum, ListName, FriendsList); // Getting all friends.

			for (const TSharedRef<FOnlineFriend>& Friend : FriendsList)
			{
				FriendsListNames.Add(Friend->GetDisplayName()); // Adding all friend's names.
			}

			Callback.ExecuteIfBound(FriendsListNames); // Invoking the delegate: the function is ready!
		}));

	return bGetFriendsSuccessful;
}