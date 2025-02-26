// © Manuel Solano
// © Alessandro Caccamo
// © Claudio Dallai

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PNetworkingBPLibrary.generated.h"

class FOnlineFriend;

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
	static bool GetFriendsList(TArray<FString>& FriendsListNames, const int32 LocalUserNum = 0);
};
