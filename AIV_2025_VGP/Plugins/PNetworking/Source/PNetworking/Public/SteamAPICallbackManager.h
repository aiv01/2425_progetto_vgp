// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// To disable "strncpy" security warnings.
#pragma warning(push)
#pragma warning(disable:4996)
#include "AI/NavigationSystemBase.h"
#include "steam/steam_api.h"
#pragma warning(pop)

#include "CoreMinimal.h"

DECLARE_DELEGATE_OneParam(FOnAvatarReadyFromSteamAPI, AvatarImageLoaded_t*)

class PNETWORKING_API SteamAPICallbackManager
{
private:
	STEAM_CALLBACK(SteamAPICallbackManager, OnImageAvatarLoadedCallback, AvatarImageLoaded_t);

public:
	FOnAvatarReadyFromSteamAPI OnAvatarReadyDelegateLocalUser;
	FOnAvatarReadyFromSteamAPI OnAvatarReadyDelegateFriendList;
	FOnAvatarReadyFromSteamAPI OnAvatarReadyFriendListData;


	SteamAPICallbackManager();
	~SteamAPICallbackManager();

};
