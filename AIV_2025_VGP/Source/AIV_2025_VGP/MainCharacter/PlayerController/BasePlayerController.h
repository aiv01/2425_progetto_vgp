#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

#include "BasePlayerController.generated.h"

UCLASS()
class AIV_2025_VGP_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack();
	void Jump();
	void ChangeWeapon(const FInputActionValue& Value);

	UPROPERTY()
	UInputMappingContext* IMC_MainCharacter;

	UPROPERTY()
	UInputAction* IA_CstmJump;

	UPROPERTY()
	UInputAction* IA_CstmMove;

	UPROPERTY()
	UInputAction* IA_CstmLook;

	UPROPERTY()
	UInputAction* IA_CstmAttack;

	UPROPERTY()
	UInputAction* IA_CstmChangeWeapon;

public:
	ABasePlayerController();
};
