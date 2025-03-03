// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#pragma once

#include "CoreMinimal.h"
#include "UserSteamData.generated.h"

USTRUCT(BlueprintType)
struct FUserSteamData
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly)
    int32 SteamID; // blueprint not supported "uint32", so we need to do some casts. 
    
    UPROPERTY(BlueprintReadOnly)
    FText UserName;
    
    UPROPERTY(BlueprintReadOnly)
    UTexture2D* UserAvatar;

    FUserSteamData() = default;

    FUserSteamData(const int32 NewSteamID, const FText NewUserName, UTexture2D* NewUserAvatar) : SteamID(NewSteamID), UserName(NewUserName), UserAvatar(NewUserAvatar) { }

};