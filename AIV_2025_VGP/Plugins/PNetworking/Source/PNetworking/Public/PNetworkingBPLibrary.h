// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "PNetworkingBPLibrary.generated.h"

class FOnlineFriend;
class CSteamID;
struct FUserSteamData;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFriendsListReady, const TArray<FString>&, FriendsListNames);

UCLASS()
class PNETWORKING_API UPNetworkingBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
#pragma region LocalUser
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static bool GetAppID(FString& AppID);
	
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static bool GetAccountName(FString& AccountName, const int32 UserID = 0);

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static UTexture2D* GetLocalUserAvatar();
#pragma endregion LocalUser
	
#pragma region Friends
    UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
    static bool GetOnlineFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum = 0);

    UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
    static bool GetAllFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum = 0);
	
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static TArray<UTexture2D*> GetFriendsAvatar();
#pragma endregion Friends
	
#pragma region General
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static TArray<FUserSteamData> GetPlayersData();
#pragma endregion General

#pragma region Debug
	// UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	// static FString GetUserNameFromSteamID(const int32 SteamID);
#pragma endregion Debug

private:
    static bool GetFriendsList(const FOnFriendsListReady& Callback, const EFriendsLists::Type Query, const int32 LocalUserNum = 0);
	static UTexture2D* GetAvatar(const CSteamID SteamID);
	static int32 GetOnlineFriendsFromFriendCount(const int32 FriendsCount);

};
