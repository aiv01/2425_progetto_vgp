// © Manuel Solano
// © Alessandro Caccamo
// © Claudio Dallai

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NetworkManagerSubsystem.generated.h"

UCLASS()
class AIV_2025_VGP_API UNetworkManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UNetworkManagerSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

};
