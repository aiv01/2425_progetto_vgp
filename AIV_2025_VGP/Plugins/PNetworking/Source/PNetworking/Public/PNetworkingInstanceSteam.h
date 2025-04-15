// Copyright Epic Games, Inc. All Rights Reserved.

// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PNetworkingInstanceSteam.generated.h"

#pragma region ForwardDeclaration

class FOnlineFriend;
class CSteamID;
struct FUserSteamData;

#pragma endregion

#pragma region Delegates

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLocalAvatarReady, const UTexture2D*, LocalAvatar);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFriendsListReady, const TArray<FString>&, FriendsListNames);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFriendsAvatarReady, const TArray<UTexture2D*>&, FriendsListAvatars);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFriendsDataReady, const TArray<FUserSteamData>&, FriendsListDatas);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSessionCreationCompleted, FName, CreatedSessionName, bool, bCreationWasSuccessfull);


#pragma endregion

UCLASS(BlueprintType, meta=(NotBlueprintable))
class PNETWORKING_API UPNetworkingInstanceSteam : public UObject
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "Steam Net Plugin management")
	static UPNetworkingInstanceSteam* GetUniqueInstance();

	UFUNCTION(BlueprintCallable, Category = "Steam Net Plugin management")
	static void DeleteUniqueInstance();

#pragma region LocalUser
	/// <summary>
	
	/// Get LocalAppID of the game. In case of develop it returns 480.
	/// </summary>
	/// <param name="AppID"> Out AppID in FString type. </param>
	/// <returns> Returns true if the operation was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem local user functions")
	bool GetAppID(FString& AppID);

	/// <summary>
	/// Get local steam account Name.
	/// </summary>
	/// <param name="AccountName"> Out AccountName in FString type. </param>
	/// <param name="UserID"> ID to look for. In default case of 0, it takes local user. </param>
	/// <returns> Returns true if the operation was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem local user functions")
	bool GetAccountName(FString& AccountName, const int32 UserID = 0);

	/// <summary>
	/// Get the local user avatar as a Callback. It returns UTexture2D* to Avatar texture.
	/// </summary>
	/// <param name="Callback"> Callback to be binded in BP/Cpp. </param>
	/// <returns> int32 flag. 0 means error, 1 means result correct, -1 means in loading waiting for STEAMAPI. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem local user functions")
	int32 GetLocalUserAvatar(const FOnLocalAvatarReady& Callback);

#pragma endregion

#pragma region FriendlistUtilityLocalUser

	/// <summary>
	/// Get all online friends names of specified user.
	/// </summary>
	/// <param name="Callback"> Fired when the search and data retreive is completed. </param>
	/// <param name="LocalUserNum"> UserNum to search its friendlist. In default case of 0, it takes local user. </param>
	/// <returns> Returns true if the inital request was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	bool GetOnlineFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum = 0);

	/// <summary>
	/// Get all friends names of specified user.
	/// </summary>
	/// <param name="Callback"> Fired when the search and data retreive is completed. </param>
	/// <param name="LocalUserNum"> UserNum to search its friendlist. In default case of 0, it takes local user. </param>
	/// <returns> Returns true if the inital request was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	bool GetAllFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum = 0);

	/// <summary>
	/// Get all local user friendlist avatars as a Callback. It returns TArray<UTexture2D>& containing all textures.
	/// </summary>
	/// <param name="Callback"> Callback to be binded in BP/Cpp. </param>
	/// <returns> int32 flag. 0 means error, 1 means result correct, -1 means in loading waiting for STEAMAPI. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	int32 GetFriendsAvatar(const FOnFriendsAvatarReady& Callback);

	/// <summary>
	/// Get complete and usable informations of all online Friends of local user as a Callback.
	/// It returns TArray<FUserSteamData>& containing all data to identify in BP a friend.
	/// </summary>
	/// <param name="bAlphabeticalSort"> If TArray elements should be alphabetically sorted using their nicknames. </param>
	/// /// <param name="Callback"> Callback to be binded in BP/Cpp. </param>
	/// <returns> int32 flag. 0 means error, 1 means result correct, -1 means in loading waiting for STEAMAPI. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	int32 GetPlayersData(const bool bAlphabeticalSort, const FOnFriendsDataReady& Callback);

#pragma endregion

#pragma region Session
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	bool RequestSessionCreation(const int32 NumberPublicConnections, const int32 NumberPrivateConnections,
		const bool bIsLANMatch, const bool bIsDedicated, const bool bShouldAdvertise, const bool bUsesPresence,
		const bool bAllowJoinViaPresenceFriendsOnly, const bool bUseLobbiesIfAvailable = true);

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	bool InviteFriend(const int32 SteamID);

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	bool InitializeOnlineCallbacks();

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	bool DeInitializeOnlineCallbacks();
#pragma endregion Session

#pragma region Debug
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	FString GetUserNameFromSteamID(const int32 SteamID);
#pragma endregion Debug

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	void CheckAndDestroyAlreadyExistingSession();


private:

	UPNetworkingInstanceSteam();
	~UPNetworkingInstanceSteam();

	static UPNetworkingInstanceSteam* NetInstanceSteamPtr;
	

	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FDelegateHandle SessionParticipantLeftDelegateHandle;
	FDelegateHandle SessionParticipantRemovedDelegateHandle;
	FDelegateHandle SessionUserInviteAcceptedDelegateHandle;
	FDelegateHandle OnNetworkFailureDelegateHandle;
	FDelegateHandle OnUnregisterLocalPlayerDelegateHandle;
	FDelegateHandle OnSessionPlayerNetworkFailureHandle;


	bool GetFriendsList(const FOnFriendsListReady& Callback, const EFriendsLists::Type Query, const int32 LocalUserNum = 0);
	UTexture2D* GetAvatar(const CSteamID SteamID, int32& QueryResult);
	int32 GetOnlineFriendsFromFriendCount(const int32 FriendsCount);
	void AlphabeticalSortFriends(TArray<FUserSteamData>& FriendsToSort);
	bool ConvertCSteamIDToFUniqueNetID(const CSteamID SteamID, FUniqueNetIdPtr& CorrespondanceNetID);
	CSteamID ConvertInt32toCSteamID(const int32 SteamID);
	void OnInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, FUniqueNetIdPtr FriendID, const FOnlineSessionSearchResult& InviteResult);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void OnCreateSessionComplete(FName NewName, bool bWasSuccessfull);
	void OnDestroySessionComplete(FName sessionName, bool bWasSuccessfull);
	void OnPlayerLeft(FName sessionName, const FUniqueNetId& uniqueIdPlayerLeft, EOnSessionParticipantLeftReason reason);
	void OnPlayerRemoved(FName sessionName, const FUniqueNetId& uniqueIdPlayerLeft);
	void OnLocalPlayerUnregistered(const FUniqueNetId& uniqueIdPlayerLeft, const bool bResult); // Da rimuovere poichè probabilmente LAN
	void OnSessionPlayerNetworkFailure(const FUniqueNetId& CrashedPlayerID, ESessionFailure::Type ErrorType);
	void DestroySession();

	// Recursive async callbacks on GameThread.
	int32 GetLocalUserAvatarRecursive(FOnLocalAvatarReady Callback);
	int32 GetFriendsAvatarRecursive(FOnFriendsAvatarReady Callback);
	int32 GetPlayerDataRecursive(const bool bAlphabeticalSort, FOnFriendsDataReady Callback);
};
