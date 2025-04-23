// • Manuel Solano
// • Alessandro Caccamo
// • Claudio Dallai

#pragma once

#include "CoreMinimal.h"
#include "UserSteamData.generated.h"

// Contains informations to retreive steam users data.
// It is made in order to use it in blueprints.

USTRUCT(BlueprintType)
struct FUserSteamData
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly)
    int32 SteamID; // Blueprint not supported "uint32", so we need to do some casts. 
    
    UPROPERTY(BlueprintReadOnly)
    FText UserName;
    
    UPROPERTY(BlueprintReadOnly)
    UTexture2D* UserAvatar;

    FUserSteamData() : SteamID(0), UserName(FText::FromString(TEXT("Unknown"))), UserAvatar(nullptr) { } // Pre-compiler "= default" does not work.
    FUserSteamData(const int32 NewSteamID, const FText NewUserName, UTexture2D* NewUserAvatar) : SteamID(NewSteamID), UserName(NewUserName), UserAvatar(NewUserAvatar) { }
    
};