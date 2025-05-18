// Fill out your copyright notice in the Description page of Project Settings.

// • Alessandro Caccamo
// • Claudio Dallai

#pragma once

// To disable "strncpy" security warnings.
#pragma warning(push)
#pragma warning(disable:4996)
#include "AI/NavigationSystemBase.h"
#include "steam/steam_api.h"
#pragma warning(pop)

#include "CoreMinimal.h"

/* 
	This class is used to bind and communicate with STEAMWORKS_API.
	It was created because OSS does not expose and wrap every Steamworks functions, such GetAvatar.
	In fact this file implements steam_api.h, which was imported by us in this folder.
	UE also uses and has the correct version of steam sdk in its files, but they're not exposed.
*/

// Delegate used to communicate with PNetworkingInstance file.
DECLARE_DELEGATE_OneParam(FOnAvatarReadyFromSteamAPI, AvatarImageLoaded_t*)

class PNETWORKING_API SteamAPICallbackManager
{
private:

	// Bind to Steamworks callbacks.
	STEAM_CALLBACK(SteamAPICallbackManager, OnImageAvatarLoadedCallback, AvatarImageLoaded_t);

public:

	// Delegates called when Steam_API notifies relative callbacks.
	FOnAvatarReadyFromSteamAPI OnAvatarReadyDelegateLocalUser;
	FOnAvatarReadyFromSteamAPI OnAvatarReadyDelegateFriendList;
	FOnAvatarReadyFromSteamAPI OnAvatarReadyFriendListData;
	FOnAvatarReadyFromSteamAPI OnAvatarReadyFriendRequested;

	SteamAPICallbackManager();
	~SteamAPICallbackManager();

};
