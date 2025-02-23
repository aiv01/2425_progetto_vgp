// Manuel Solano
// Alessandro Caccamo
// Claudio Dallai

#include "NETGameInstance.h"
#include "OnlineSubsystem.h"

UNETGameInstance::UNETGameInstance()
{
    UE_LOG(LogTemp, Warning, TEXT("GameInstance Constructor called"));
    onlineSubsystemReference = nullptr;
}

void UNETGameInstance::Init()
{
    Super::Init();
    UE_LOG(LogTemp, Warning, TEXT("GameInstance Init called"));

    InitializeOnlineSubsystem();
}

void UNETGameInstance::Shutdown()
{
    Super::Shutdown();
    UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown called"));

}

void UNETGameInstance::InitializeOnlineSubsystem()
{
    UE_LOG(LogTemp, Warning, TEXT("GameInstance trying access online subsystem"));
    onlineSubsystemReference = IOnlineSubsystem::Get();

    if (onlineSubsystemReference)
    {
        UE_LOG(LogTemp, Warning, TEXT("Online subsystem found!"));
    }
}

IOnlineSubsystem* UNETGameInstance::GetOnlineSubsystem()
{
    return onlineSubsystemReference;
}
