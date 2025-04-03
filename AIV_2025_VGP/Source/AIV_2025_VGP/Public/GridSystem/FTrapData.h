// Fill out your copyright notice in the Description page of Project Settings.
// Caponera Marco
// Cimino Alberto
// Sanzogni Gabriele
// Vernone Michele
#pragma once

#include "CoreMinimal.h"
#include "ETrapType.h"
#include "FTrapData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct AIV_2025_VGP_API FTrapData: public FTableRowBase
{
	GENERATED_BODY()
public:

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Type", MakeStructureDefaultValue="NewEnumerator0"))
	TSet<TEnumAsByte<ETrapType>> Type;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Mesh"))
	TObjectPtr<UStaticMesh> Mesh;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Trap Class", MakeStructureDefaultValue="None"))
	TObjectPtr<UClass> TrapClass;

};

