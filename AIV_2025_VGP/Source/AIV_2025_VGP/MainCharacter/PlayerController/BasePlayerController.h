// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "../BasePlayer.h"

#include "BasePlayerController.generated.h"

UCLASS()
class AIV_2025_VGP_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

#pragma region Properties
private:
	UPROPERTY()
	UInputMappingContext* IMC_MainCharacter;

	UPROPERTY()
	UInputAction* IA_CstmJump;

	UPROPERTY()
	UInputAction* IA_CstmMove;

	UPROPERTY()
	UInputAction* IA_CstmLook;

	UPROPERTY()
	UInputAction* IA_CstmPrimaryAttack;

	UPROPERTY()
	UInputAction* IA_CstmSecondaryAttack;
	
	UPROPERTY()
	UInputAction* IA_CstmChangeWeapon;
#pragma endregion Properties

#pragma region Functions
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	//Input function
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PrimaryAttack();
	void SecondaryAttack();
	void Jump();
	void ChangeWeapon(const FInputActionValue& Value);

	//Init function
	void FindInputActions();

public:
	ABasePlayerController(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintCallable)
	TArray<ABasePlayer*> GetAllBasePlayers() const;
#pragma endregion Functions
};
