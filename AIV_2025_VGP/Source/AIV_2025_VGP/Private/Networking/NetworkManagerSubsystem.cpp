// Manuel Solano
// Alessandro Caccamo
// Claudio Dallai

#include "Networking/NetworkManagerSubsystem.h"
#include "OnlineSubsystem.h"
#include "NETGameInstance.h"

UNetworkManagerSubsystem::UNetworkManagerSubsystem()
{
    UE_LOG(LogTemp, Warning, TEXT("NetworkManagerSubsystem Constructor called"));
    gameInstance = nullptr;
}

void UNetworkManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Warning, TEXT("NetworkManagerSubsystem Initialized called"));

    gameInstance = Cast<UNETGameInstance>(GetWorld()->GetGameInstance());

    if (gameInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("NetworkManagerSubsystem found gameInstance"));
    }
}

void UNetworkManagerSubsystem::Deinitialize()
{
    Super::Deinitialize();
    UE_LOG(LogTemp, Warning, TEXT("NetworkManagerSubsystem Deinitialized called"));
}

FString UNetworkManagerSubsystem::GetAppID() const
{
    FString currentGameID = "gameInstance invalid";

    if (gameInstance)
    {
        IOnlineSubsystem* onlineSubsystem = gameInstance->GetOnlineSubsystem();
        if (onlineSubsystem)
        {
            currentGameID = onlineSubsystem->GetAppId();
            UE_LOG(LogTemp, Warning, TEXT("Online subsystem is valid and using AppID: %s"), *currentGameID);
        }
    }

    return currentGameID;
}

