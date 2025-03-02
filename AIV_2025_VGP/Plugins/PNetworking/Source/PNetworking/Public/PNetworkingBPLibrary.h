// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "PNetworkingBPLibrary.generated.h"

class FOnlineFriend;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFriendsListReady, const TArray<FString>&, FriendsListNames);

UCLASS()
class PNETWORKING_API UPNetworkingBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static bool GetAppID(FString& AppID);

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static bool GetAccountName(FString& AccountName, const int32 UserID = 0);

    UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
    static bool GetOnlineFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum = 0);

    UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
    static bool GetAllFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum = 0);

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static UTexture2D* GetAvatar();

private:
    static bool GetFriendsList(const FOnFriendsListReady& Callback, const EFriendsLists::Type Query, const int32 LocalUserNum = 0);

};
