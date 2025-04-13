// Fill out your copyright notice in the Description page of Project Settings.

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
	OnAvatarReadyDelegateLocalUser.ExecuteIfBound(callback);
	OnAvatarReadyDelegateFriendList.ExecuteIfBound(callback);
	OnAvatarReadyFriendListData.ExecuteIfBound(callback);
}
