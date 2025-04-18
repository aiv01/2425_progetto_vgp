// Copyright Epic Games, Inc. All Rights Reserved.

// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "OnlineSessionSettings.h"
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

// Non usato ora sarebbe bene riuscire a riusarlo.
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSessionCreationCompleted, FName, CreatedSessionName, bool, bCreationWasSuccessfull);

#pragma endregion

UCLASS(BlueprintType, meta=(NotBlueprintable))
class PNETWORKING_API UPNetworkingInstanceSteam : public UObject
{
	GENERATED_BODY()

public:

#pragma region SingletonePluginManagement

	/// <summary>
	/// Check if a singletone static istance already exists.
	/// If not, it creates one, save it in a raw pointer and add it to root (prevent Garbage Collector) and return it.
	/// If it already exists, return it.
	/// </summary>
	/// <returns> Unique istance of this class. </returns>
	UFUNCTION(BlueprintCallable, Category = "Steam Net Plugin management")
	static UPNetworkingInstanceSteam* GetUniqueInstance();

	/// <summary>
	/// Remove from root the unique istance of this class.
	/// Invalid its pointer.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Steam Net Plugin management")
	static void DeleteUniqueInstance();

#pragma endregion SingletonePluginManagement

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

#pragma endregion LocalUser

#pragma region FriendlistUtilityLocalUser

	/// <summary>
	/// Get Steam username from a SteamID.
	/// </summary>
	/// <param name="SteamID"> SteamID to get the username from. It is int32 type in order to be used in blueprints. </param>
	/// <returns> Steam username of specified SteamID, or "" if not found/invalid. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	FString GetUsernameFromSteamID(const int32 SteamID);

	/// <summary>
	/// Get all online friends names of specified user.
	/// </summary>
	/// <param name="Callback"> Fired when the search and data retreive is completed. </param>
	/// <param name="LocalUserNum"> UserNum to search its friendlist. In default case of 0, it takes local user. </param>
	/// <returns> Returns true if the inital request was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	bool GetOnlineFriendListNames(const FOnFriendsListReady& Callback, const int32 LocalUserNum = 0);

	/// <summary>
	/// Get all friends names of specified user.
	/// </summary>
	/// <param name="Callback"> Fired when the search and data retreive is completed. </param>
	/// <param name="LocalUserNum"> UserNum to search its friendlist. In default case of 0, it takes local user. </param>
	/// <returns> Returns true if the inital request was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	bool GetAllFriendListNames(const FOnFriendsListReady& Callback, const int32 LocalUserNum = 0);

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

#pragma endregion FriendlistUtilityLocalUser

#pragma region SessionManagement

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Session functions")
	bool RequestSessionCreation(const int32 NumberPublicConnections, const int32 NumberPrivateConnections,
								const bool bIsLANMatch, const bool bIsDedicated, const bool bShouldAdvertise, const bool bUsesPresence,
								const bool bAllowJoinViaPresenceFriendsOnly, const bool bUseLobbiesIfAvailable = true);

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Session functions")
	bool InviteFriend(const int32 SteamID);

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	void TravelBack();

#pragma endregion SessionManagement

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem")
	bool InitializeOnlineCallbacks();

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem")
	bool DeInitializeOnlineCallbacks();

#pragma region Debug

#pragma endregion Debug

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	void CheckAndDestroyAlreadyExistingSession();



private:

#pragma region SpecialMemberFunctions

	UPNetworkingInstanceSteam();
	~UPNetworkingInstanceSteam();

#pragma endregion SpecialMemberFunctions

	static UPNetworkingInstanceSteam* NetInstanceSteamPtr;
	FOnlineSessionSettings NewSessionSettings;
	FOnlineSessionSearchResult CurrentInviteResult;

#pragma region DelegatesHandle

	// Fired when a new session has been created.
	FDelegateHandle CreateSessionCompleteDelegateHandle; 

	// Fired when successfully joined an existing session.
	FDelegateHandle JoinSessionCompleteDelegateHandle; 

	// Fired when User accepts an invite.
	FDelegateHandle SessionUserInviteAcceptedDelegateHandle; 
	
	/* Fired when a network failure is called from GameInstance (session socket is invalid).
	Usually called on clients when host crashes for any reason. */
	FDelegateHandle OnNetworkFailureDelegateHandle; 

	// Fired to server when a client connection is lost.
	FDelegateHandle OnSessionPlayerNetworkFailureHandle; 

	FDelegateHandle DestroySessionCompleteDelegateHandle; 
	FDelegateHandle OnClientDestroySessionCompleteHandle;
	FDelegateHandle OnClientDestroySessionCompleteFromLobbyHandle;

#pragma endregion DelegatesHandle

	bool GetFriendList(const FOnFriendsListReady& Callback, const EFriendsLists::Type Query, const int32 LocalUserNum = 0);
	UTexture2D* GetAvatar(const CSteamID SteamID, int32& QueryResult);
	int32 GetOnlineFriendsFromFriendCount(const int32 FriendsCount);
	void AlphabeticalSortFriends(TArray<FUserSteamData>& FriendsToSort);
	bool ConvertCSteamIDToFUniqueNetID(const CSteamID SteamID, FUniqueNetIdPtr& CorrespondanceNetID);
	CSteamID ConvertInt32toCSteamID(const int32 SteamID);
	void DestroySession();
	void CreateSession();

#pragma region CallbackFunctions

	void OnCreateSessionComplete(FName NewName, bool bWasSuccessfull);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, FUniqueNetIdPtr FriendID, const FOnlineSessionSearchResult& InviteResult);

	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void OnPlayerInSessionNetworkFailure(const FUniqueNetId& CrashedPlayerID, ESessionFailure::Type ErrorType);

	void OnDestroySessionComplete(FName sessionName, bool bWasSuccessfull);
	void OnClientDestroySessionComplete(FName sessionName, bool bWasSuccessfull);
	void OnClientDestroySessionCompleteFromLobby(FName sessionName, bool bWasSuccessfull);

#pragma endregion CallbackFunctions

#pragma region SteamworksFunctions

	// Recursive async callbacks on GameThread. Used to get avatars from async STEAMWORKS_API sdk.
	int32 GetLocalUserAvatarRecursive(TSharedPtr<FOnLocalAvatarReady> Callback);
	int32 GetFriendsAvatarRecursive(TSharedPtr<FOnFriendsAvatarReady> Callback);
	int32 GetPlayerDataRecursive(const bool bAlphabeticalSort, TSharedPtr<FOnFriendsDataReady> Callback);

#pragma endregion SteamworksFunctions
	
};
