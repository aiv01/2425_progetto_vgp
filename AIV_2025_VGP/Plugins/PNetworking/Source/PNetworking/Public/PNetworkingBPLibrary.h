// Copyright Epic Games, Inc. All Rights Reserved.

// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PNetworkingBPLibrary.generated.h"

#pragma region ForwardDeclaration

class FOnlineFriend;
class CSteamID;
struct FUserSteamData;

#pragma endregion

#pragma region Delegates

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFriendsListReady, const TArray<FString>&, FriendsListNames);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSessionCreationCompleted, FName, CreatedSessionName, bool, bCreationWasSuccessfull);

#pragma endregion

UCLASS()
class PNETWORKING_API UPNetworkingBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

#pragma region LocalUser

	/// <summary>
	/// Get LocalAppID of the game. In case of develop it returns 480.
	/// </summary>
	/// <param name="AppID"> Out AppID in FString type. </param>
	/// <returns> Returns true if the operation was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem local user functions")
	static bool GetAppID(FString& AppID);
	
	/// <summary>
	/// Get local steam account Name.
	/// </summary>
	/// <param name="AccountName"> Out AccountName in FString type. </param>
	/// <param name="UserID"> ID to look for. In default case of 0, it takes local user. </param>
	/// <returns> Returns true if the operation was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem local user functions")
	static bool GetAccountName(FString& AccountName, const int32 UserID = 0);

	/// <summary>
	/// Get local steam account Avatar.
	/// </summary>
	/// <returns> Returns pointer of UTexture2D if found, else nullptr. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem local user functions")
	static UTexture2D* GetLocalUserAvatar();

#pragma endregion
	
#pragma region FriendlistUtilityLocalUser

    /// <summary>
    /// Get all online friends names of specified user.
    /// </summary>
    /// <param name="Callback"> Fired when the search and data retreive is completed. </param>
    /// <param name="LocalUserNum"> UserNum to search its friendlist. In default case of 0, it takes local user. </param>
    /// <returns> Returns true if the inital request was successfull. </returns>
    UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
    static bool GetOnlineFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum = 0);

    /// <summary>
    /// Get all friends names of specified user.
    /// </summary>
    /// <param name="Callback"> Fired when the search and data retreive is completed. </param>
    /// <param name="LocalUserNum"> UserNum to search its friendlist. In default case of 0, it takes local user. </param>
    /// <returns> Returns true if the inital request was successfull. </returns>
    UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
    static bool GetAllFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum = 0);
	
	/// <summary>
	/// Get all friends Avatars of local user.
	/// </summary>
	/// <returns> TArray containing pointers of UTexture2D if found, else nullptr. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	static TArray<UTexture2D*> GetFriendsAvatar();

	/// <summary>
	/// Get complete and usable informations of all online Friends of local user.
	/// </summary>
	/// <param name="bAlphabeticalSort"> If TArray elements should be alphabetically sorted using their nicknames. </param>
	/// <returns> FUserSteamData contains nickname, avatar, accountID(int32 version of a CSteamID). </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	static TArray<FUserSteamData> GetPlayersData(const bool bAlphabeticalSort);

#pragma endregion

#pragma region Session
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static bool RequestSessionCreation(const int32 NumberPublicConnections, const int32 NumberPrivateConnections, 
									   const bool bIsLANMatch, const bool bIsDedicated, const bool bShouldAdvertise, const bool bUsesPresence,
									   const bool bAllowJoinViaPresenceFriendsOnly, const bool bUseLobbiesIfAvailable = true);

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static bool InviteFriend(const int32 SteamID);

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static bool InitializeOnlineCallbacks();
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static bool DeInitializeOnlineCallbacks();
#pragma endregion Session

#pragma region Debug
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static FString GetUserNameFromSteamID(const int32 SteamID);
#pragma endregion Debug

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static void DestroySession();

private:

	static FDelegateHandle CreateSessionCompleteDelegateHandle;
	static FDelegateHandle JoinSessionCompleteDelegateHandle;
	static FDelegateHandle DestroySessionCompleteDelegateHandle;
	static FDelegateHandle SessionParticipantLeftDelegateHandle;
	static FDelegateHandle SessionUserInviteAcceptedDelegateHandle;
	static FDelegateHandle OnNetworkFailureDelegateHandle;

    static bool GetFriendsList(const FOnFriendsListReady& Callback, const EFriendsLists::Type Query, const int32 LocalUserNum = 0);
	static UTexture2D* GetAvatar(const CSteamID SteamID);
	static int32 GetOnlineFriendsFromFriendCount(const int32 FriendsCount);
	static void AlphabeticalSortFriends(TArray<FUserSteamData>& FriendsToSort);
	static bool ConvertCSteamIDToFUniqueNetID(const CSteamID SteamID, FUniqueNetIdPtr& CorrespondanceNetID);
	static CSteamID ConvertInt32toCSteamID(const int32 SteamID);
	static void OnInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, FUniqueNetIdPtr FriendID, const FOnlineSessionSearchResult& InviteResult);
	static void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	static void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	static void OnCreateSessionComplete(FName NewName, bool bWasSuccessfull);
	static void OnDestroySessionComplete(FName sessionName, bool bWasSuccessfull);
	static void OnPlayerLeft(FName sessionName, const FUniqueNetId& uniqueIdPlayerLeft, EOnSessionParticipantLeftReason reason);
	static void CheckAndDestroyAlreadyExistingSession();

};
