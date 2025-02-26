#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PNetworkingBPLibrary.generated.h"

UCLASS()
class PNETWORKING_API UPNetworkingBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static FString GetAppID();

	UFUNCTION(BlueprintCallable, Category = "Online Subsystem Metadata")
	static FString GetAccountName(const uint32 UserID = 0);
};
