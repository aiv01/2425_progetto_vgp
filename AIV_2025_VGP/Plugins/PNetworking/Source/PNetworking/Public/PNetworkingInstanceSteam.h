// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// • Manuel Solano
// • Alessandro Caccamo
// • Claudio Dallai

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "OnlineSessionSettings.h"
#include "SessionCreationParameters.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PNetworkingInstanceSteam.generated.h"

#pragma region ForwardDeclaration

class FOnlineFriend;
class CSteamID;
struct FUserSteamData;
struct FSessionCreationParameters;
enum ELocalSessionState : uint8;

#pragma endregion

#pragma region Delegates

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLocalAvatarReady, const UTexture2D*, LocalAvatar);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFriendsListReady, const TArray<FString>&, FriendsListNames);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFriendsAvatarReady, const TArray<UTexture2D*>&, FriendsListAvatars);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestedFriendAvatarReady, const UTexture2D*, RequestedFriendAvatar);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFriendsDataReady, const TArray<FUserSteamData>&, FriendsListDatas);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSessionParametersUpdateReady, FName, SessionName, bool, bWasSuccessfull);

#pragma endregion

#define EMPTY_FSTRING ""

UCLASS(BlueprintType, meta=(NotBlueprintable))
class PNETWORKING_API UPNetworkingInstanceSteam : public UObject
{
	GENERATED_BODY()

public:

#pragma region SingletonPluginManagement

	/// <summary>
	/// Check if a singleton static instance already exists.
	/// If not, it creates one, save it in a raw pointer and add it to root (prevent Garbage Collector) and return it.
	/// If it already exists, return it.
	/// </summary>
	/// <returns> Unique instance of this class. </returns>
	UFUNCTION(BlueprintCallable, Category = "Steam Net Plugin management")
	static UPNetworkingInstanceSteam* GetUniqueInstance();

	/// <summary>
	/// Remove from root the unique instance of this class.
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
	/// Get LocalCSteamID converted into int32 to be used in blueprint.
	/// </summary>
	/// <param name="OutSteamID"> Out converted CSteamID. </param>
	/// <returns> Returns true if the operation was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem local user functions")
	bool GetLocalCSteamID(int32& OutSteamID);

	/// <summary>
	/// Get local Steam account name.
	/// </summary>
	/// <param name="AccountName"> Out AccountName in FString type. </param>
	/// <param name="UserID"> ID to look for. In default case of 0, it takes local user. </param>
	/// <returns> Returns true if the operation was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem local user functions")
	bool GetAccountName(FString& AccountName, const int32 UserID = 0);

	/// <summary>
	/// Get the local user avatar as a callback. It returns UTexture2D* to Avatar texture.
	/// </summary>
	/// <param name="Callback"> Callback to be bound in BP/C++. </param>
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
	/// Get Friend Avatar from SteamID.
	/// </summary>
	/// <param name="SteamID"> CSteamID to get Avatar from. It is int32 type in order to be used in blueprints. </param>
	/// <param name="Callback"> Fired when the search and data retreive is completed. </param>
	/// <returns> int32 flag. 0 means error, 1 means result correct, -1 means in loading waiting for STEAMAPI. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	int32 GetAvatarFromSteamID(const int32 SteamID, const FOnRequestedFriendAvatarReady& Callback);

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
	/// <param name="Callback"> Callback to be bound in BP/C++. </param>
	/// <returns> int32 flag. 0 means error, 1 means result correct, -1 means in loading waiting for STEAMAPI. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	int32 GetFriendsAvatar(const FOnFriendsAvatarReady& Callback);

	/// <summary>
	/// Get complete and usable informations of all online Friends of local user as a Callback.
	/// It returns TArray<FUserSteamData>& containing all data to identify in BP a friend.
	/// </summary>
	/// <param name="bAlphabeticalSort"> If TArray elements should be alphabetically sorted using their nicknames. </param>
	/// <param name="Callback"> Callback to be bound in BP/C++. </param>
	/// <returns> int32 flag. 0 means error, 1 means result correct, -1 means in loading waiting for STEAMAPI. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem friendlist utility functions")
	int32 GetPlayersData(const bool bAlphabeticalSort, const FOnFriendsDataReady& Callback);

#pragma endregion FriendlistUtilityLocalUser

#pragma region SessionManagement

	/// <summary>
	/// Request the creation of a new session, using localPlayer as new Host.
	/// Checks for an existing old session, and if found, delete it before creating the new one.
	/// </summary>
	/// <param name="SessionCreationParameters"> Struct exposed in blueprint containing all datas necessary to create a session. </param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Session functions")
	bool RequestSessionCreation(struct FSessionCreationParameters SessionCreationParameters);

	/// <summary>
	/// Invite a friend to an existing session.
	/// </summary>
	/// <param name="SteamID"> SteamID to send invite to. The type is int32 in order to be used in blueprints. </param>
	/// <returns> Returns True if invite request was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Session functions")
	bool InviteFriend(const int32 SteamID);

	/// <summary>
	/// Travel back to a default map and quit current session.
	/// </summary>
	/// <param name="TravelBackMapPath"> Absolute path (/game/..) of map to travel to. </param>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Session functions")
	void QuitSession(const FString& TravelBackMapPath);

	/// <summary>
	/// Get current session parameters.
	/// </summary>
	/// <param name="SessionParameters"> Session parameters retreived. </param>
	/// <returns> Returns True if retreiving parameters was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Session functions")
	bool GetSessionParameters(FGetSessionParameters& SessionParameters) const;

	/// <summary>
	/// Update current session parameters. Ensure to be Authority.
	/// </summary>
	/// <param name="SessionParameters"> Session parameters. </param>
	/// <param name="Callback"> Callback invoked when update is finished. </param>
	/// <returns> Returns True if parameters request was successfull. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Session functions")
	bool UpdateSessionParameters_AuthorityOnly(const FUpdateSessionParameters& SessionParameters, const FOnSessionParametersUpdateReady& Callback);
	
	/// <summary>
	/// Check is session is valid and joinable. 
	/// Does not check current number of players (if session is full, joins are not permitted by default).
	/// </summary>
	/// <returns> Returns True if session is valid and joinable using invites/presence. </returns>
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Session functions")
	bool IsSessionJoinable() const;

#pragma endregion SessionManagement

private:

#pragma region PrivateVariables
	
	// Static flag to check if module is ready. It is used because of the first constructor called by UE itself. 
	// This plugin is not ready in this case.
	static bool bIsModuleReady;

	// Unique instance of this class.
	static UPNetworkingInstanceSteam* NetInstanceSteamPtr;

	// Settings established of current session, accessible by the host, set during creation.
	// They're not updated. Use "GetSessionParameters()" to get them.
	FOnlineSessionSettings TempCreationSessionSettings;

	// Datas communicated by last invite acception.
	FOnlineSessionSearchResult LastInviteResult;

	// Last map path to travel after SessionCreation.
	FString MapPathToTravel;

	// Temp var used to checks during invites (to distinguish between inSession/outSession). DO NOT USE IT.
	ELocalSessionState TempPrevSessionState;

#pragma endregion PrivateVariables

#pragma region SpecialMemberFunctions

	UPNetworkingInstanceSteam();
	~UPNetworkingInstanceSteam();

#pragma endregion SpecialMemberFunctions

#pragma region DelegatesHandle
	
	// Delegate handles used to save callbacks registration, or unregister them.
	FDelegateHandle CreateSessionCompleteDelegateHandle; 
	FDelegateHandle JoinSessionCompleteDelegateHandle; 
	FDelegateHandle SessionUserInviteAcceptedDelegateHandle; 
	FDelegateHandle OnNetworkFailureDelegateHandle; 
	FDelegateHandle OnDestroySessionCompleteFromNewHostingUserHandle; 
	FDelegateHandle OnClientDestroySessionCompleteHandle;
	FDelegateHandle OnClientNewInviteAcceptionDestroySessionCompleteHandle;
	FDelegateHandle OnSessionParametersUpdateReadyDelegateHandle;

#pragma endregion DelegatesHandle

#pragma region PrivateUtilityFunctions

	// Friendlist.
	int32 GetOnlineFriendsFromFriendCount(const int32 FriendsCount);
	bool GetFriendList(const FOnFriendsListReady& Callback, const EFriendsLists::Type Query, const int32 LocalUserNum = 0);
	void AlphabeticalSortFriends(TArray<FUserSteamData>& FriendsToSort);

	// Utility and identification.
	bool ConvertCSteamIDToFUniqueNetID(const CSteamID SteamID, FUniqueNetIdPtr& CorrespondanceNetID);
	CSteamID ConvertInt32toCSteamID(const int32 SteamID);

	// Session.
	bool HandleOldSessionIfExisting();
	void DestroySession();
	void CreateSession();

	// Plugin instance management.
	bool InitializeNetworkingInstance();
	void DeInitializeNetworkingInstance();

#pragma endregion PrivateUtilityFunctions

#pragma region CallbackFunctions

	// Fired when a new session has been created.
	void OnCreateSessionComplete(FName NewName, bool bWasSuccessfull);

	// Fired when User accepts an invite.
	void OnInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, FUniqueNetIdPtr FriendID, const FOnlineSessionSearchResult& InviteResult);

	// Fired when successfully joined an existing session.
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/* Fired when a network failure is called from GameInstance (session socket is invalid).
	Usually called on clients when host crashes for any reason. */
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	
	/* Fired when User wants to create a new Session (becoming host).
	Old session, if existing, has been deleted in order to prevent errors. */
	void OnDestroySessionCompleteFromNewHostingUser(FName SessionName, bool bWasSuccessfull);

	// Fired when client needs to destroy and unregister local session datas, due to crash or quit.
	void OnClientDestroySessionComplete(FName SessionName, bool bWasSuccessfull);

	// Fired when client needs to destroy and unregister local session datas, due to invite acception to another session.
	void OnClientNewInviteAcceptionDestroySessionComplete(FName SessionName, bool bWasSuccessfull);

#pragma endregion CallbackFunctions

#pragma region SteamworksFunctions

	UTexture2D* GetAvatar(const CSteamID SteamID, int32& QueryResult);

	// Recursive async callbacks on GameThread. Used to get avatars from async STEAMWORKS_API sdk.
	int32 GetLocalUserAvatarRecursive(TSharedPtr<FOnLocalAvatarReady> Callback);
	int32 GetRequestedFriendAvatarRecursive(const CSteamID SteamID, TSharedPtr<FOnRequestedFriendAvatarReady> Callback);
	int32 GetFriendsAvatarRecursive(TSharedPtr<FOnFriendsAvatarReady> Callback);
	int32 GetPlayerDataRecursive(const bool bAlphabeticalSort, TSharedPtr<FOnFriendsDataReady> Callback);

#pragma endregion SteamworksFunctions
	
};
