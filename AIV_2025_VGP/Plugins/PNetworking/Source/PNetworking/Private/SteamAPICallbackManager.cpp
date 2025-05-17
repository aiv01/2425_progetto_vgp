// Fill out your copyright notice in the Description page of Project Settings.

// • Alessandro Caccamo
// • Claudio Dallai

#include "SteamAPICallbackManager.h"

SteamAPICallbackManager::SteamAPICallbackManager()
{
}

SteamAPICallbackManager::~SteamAPICallbackManager()
{
}

// No double declaration + implementation to work as STEAM_MACRO callback.
void SteamAPICallbackManager::OnImageAvatarLoadedCallback(AvatarImageLoaded_t* callback)
{
	// If something is bound to the delegates, call it. 
	// These delegates are triggered when STEAM_API is triggered by AvatarImageLoaded callback.

	OnAvatarReadyDelegateLocalUser.ExecuteIfBound(callback);
	OnAvatarReadyDelegateFriendList.ExecuteIfBound(callback);
	OnAvatarReadyFriendListData.ExecuteIfBound(callback);
	OnAvatarReadyFriendRequested.ExecuteIfBound(callback);
}
