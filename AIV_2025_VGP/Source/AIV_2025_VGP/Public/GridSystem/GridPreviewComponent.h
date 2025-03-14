#pragma once

#include "CoreMinimal.h"
#include "GridProcessorComponent.h"
#include "GridPreviewComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class AIV_2025_VGP_API UGridPreviewComponent : public UGridProcessorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Preview/Material")
	UMaterial* PreviewMaterial;
	//TObjectPtr<UMaterial> PreviewMaterial;
	
	void ShowPreview(FGridSurface* GridSurface);
	
};
