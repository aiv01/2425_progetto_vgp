// Fabrizio Conni

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HealthSystemFunctions.generated.h"

UCLASS()
class HEALTHSYSTEM_API UHealthSystemFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "HealthSystem|Functions",
		meta=(Tooltip="Use Positive Value In Damage"))
	static void MakeDamage(float Damage, AActor* TargetActor );

	static void MakeDamage_Internal(float Damage, AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "HealthSystem|Functions",
	meta=(Tooltip="Use Positive Value In Heal"))
	static void Healing(float Heal, AActor* TargetActor);

	static void Healing_Internal(float Heal, AActor* TargetActor);

	static void FriendCanRevive_Internal(AActor* TargetActor, bool bNewStatus, AActor* SelfRef);
};
