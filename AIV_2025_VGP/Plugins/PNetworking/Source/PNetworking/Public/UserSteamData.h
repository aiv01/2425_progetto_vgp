// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserSteamData.generated.h"


USTRUCT(BlueprintType)
struct FUserSteamData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 SteamID;
    UPROPERTY(BlueprintReadOnly)
    FText UserName;
    UPROPERTY(BlueprintReadOnly)
    UTexture2D* UserAvatar;


    FUserSteamData() : SteamID(0), UserName(FText::FromString(TEXT(""))), UserAvatar(nullptr) {}

    FUserSteamData(int32 NewSteamID, FText NewUserName, UTexture2D* NewUserAvatar) :
        SteamID(NewSteamID), UserName(NewUserName), UserAvatar(NewUserAvatar) 
    {
    }
};