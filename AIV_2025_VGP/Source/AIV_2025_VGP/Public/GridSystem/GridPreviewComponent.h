
#pragma once

#include "CoreMinimal.h"
#include "GridSystem/GridProcessorComponent.h"
#include "GridPreviewComponent.generated.h"

/**
 * 
 */
UCLASS()
class AIV_2025_VGP_API UGridPreviewComponent : public UGridProcessorComponent
{
	GENERATED_BODY()

public:
	
	void ShowPreview(const FVector Point);
	
};
