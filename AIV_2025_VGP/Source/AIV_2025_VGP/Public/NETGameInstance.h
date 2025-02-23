// Manuel Solano
// Alessandro Caccamo
// Claudio Dallai

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NETGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AIV_2025_VGP_API UNETGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	class IOnlineSubsystem* onlineSubsystemReference;
	
public:
	UNETGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	void InitializeOnlineSubsystem();
	IOnlineSubsystem* GetOnlineSubsystem();
};
