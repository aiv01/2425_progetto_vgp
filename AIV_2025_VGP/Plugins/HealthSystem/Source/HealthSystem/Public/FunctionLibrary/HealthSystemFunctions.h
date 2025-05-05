// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HealthSystemFunctions.generated.h"

/**
 * 
 */
UCLASS()
class HEALTHSYSTEM_API UHealthSystemFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "HealthSystem|Functions",
		meta=(Tooltip="Use Positive Value In Damage"))
	static void MakeDamage(float Damage, AActor* TargetActor);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "HealthSystem|Functions",
	meta=(Tooltip="Use Positive Value In Heal"))
	static void Healing(float Heal, AActor* TargetActor);
};
