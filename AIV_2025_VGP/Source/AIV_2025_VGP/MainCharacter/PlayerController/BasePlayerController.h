// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "../Interfaces/I_MappingsInterface.h"
#include "BasePlayerController.generated.h"
UCLASS()
class AIV_2025_VGP_API ABasePlayerController : public APlayerController, public II_MappingsInterface
{
	GENERATED_BODY()

#pragma region Properties
private:
	UPROPERTY()
	UInputMappingContext* IMC_MainCharacter;

	UPROPERTY()
	UInputMappingContext* IMC_MainCharacterTraps;

	UPROPERTY()
	UInputMappingContext* IMC_MainCharacterCombat;
	
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
	
	UPROPERTY()
	UInputAction* IA_CstmRevive;

	UPROPERTY()
	UInputAction* IA_CstmToggleTrapMode;

	UPROPERTY()
	UInputAction* IA_CstmPlaceTrap;

	UPROPERTY()
	UInputAction* IA_CstmSwapTrap;

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
	void Revive();
	void ToggleTrapMode();
	void PlaceTrap(); 
	void SwapTrap(const FInputActionValue& Value);

	//Init function
	void FindInputActions();

	//Internal Mappings Functions
	//true to add input mapping, false to remove it
	bool ManageInputMappingContext(const UInputMappingContext* InputMapping, bool Add);

public:
	ABasePlayerController(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetAllBasePlayers() const;

#pragma region MappingsInterface

	UFUNCTION(BlueprintCallable)
	virtual bool SwitchToTrapMode() override;

	UFUNCTION(BlueprintCallable)
	virtual bool SwitchToCombatMode() override;

#pragma endregion

#pragma endregion Functions
};
