// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/I_PlayerInput.h"
#include "Logging/LogMacros.h"
#include "../Public/HealthSystem/I_HealthSystem.h"
#include "BasePlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ABasePlayer : public ACharacter, public II_PlayerInput, public II_HealthSystem
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

public:
	ABasePlayer();

	virtual void Move_Implementation(const FVector2D& Value) override;
	virtual void Look_Implementation(const FVector2D& Value) override;
	virtual void PrimaryAttack_Implementation() override;
	virtual void SecondaryAttack_Implementation() override;
	virtual void ChangeWeapon_Implementation(bool bForward) override;

	virtual void AddHealth_Implementation(float Amount) override;
	virtual void RemoveHealth_Implementation(float Amount) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MontageToPlay;

protected:
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/* Returns CameraBoom subobject */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/* Returns FollowCamera subobject */
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


};
