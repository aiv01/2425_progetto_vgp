// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#include "BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../Interfaces/I_PlayerInput.h"
#include "Kismet/GameplayStatics.h"
#include "../BasePlayer.h"

ABasePlayerController::ABasePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FindInputActions();
	
}

TArray<AActor*> ABasePlayerController::GetAllBasePlayers() const
{
		TArray<AActor*> AllBasePlayer;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlayer::StaticClass(), AllBasePlayer);
		return AllBasePlayer;
}


void ABasePlayerController::FindInputActions()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_MainCharacterAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IMC_MainCharacter.IMC_MainCharacter"));
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_MainCharacterTrapsAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IMC_MainCharacterTraps.IMC_MainCharacterTraps"));
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_MainCharacterCombatAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IMC_MainCharacterCombat.IMC_MainCharacterCombat"));
	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmJump.IA_CstmJump"));
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmMove.IA_CstmMove"));
	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmLook.IA_CstmLook"));
	static ConstructorHelpers::FObjectFinder<UInputAction> PrimaryAttackActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmPrimaryAttack.IA_CstmPrimaryAttack"));
	static ConstructorHelpers::FObjectFinder<UInputAction> ChangeWeaponActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmChangeWeapon.IA_CstmChangeWeapon"));
	static ConstructorHelpers::FObjectFinder<UInputAction> SecondaryAttackActionAssets(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmSecondaryAttack.IA_CstmSecondaryAttack"));
	static ConstructorHelpers::FObjectFinder<UInputAction> ReviveActionAssets(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmRevive.IA_CstmRevive"));
	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleTrapModeActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmToggleTrapMode.IA_CstmToggleTrapMode"));
	static ConstructorHelpers::FObjectFinder<UInputAction> PlaceTrapActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmPlaceTrap.IA_CstmPlaceTrap"));
	static ConstructorHelpers::FObjectFinder<UInputAction> SwapTrapActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmSwapTrap.IA_CstmSwapTrap"));

	if (IMC_MainCharacterAsset.Succeeded())
	{
		IMC_MainCharacter = IMC_MainCharacterAsset.Object;		
	}
	if (IMC_MainCharacterTrapsAsset.Succeeded())
	{
		IMC_MainCharacterTraps = IMC_MainCharacterTrapsAsset.Object;
	}
	if (IMC_MainCharacterCombatAsset.Succeeded())
	{
		IMC_MainCharacterCombat = IMC_MainCharacterCombatAsset.Object;
	}
	if (JumpActionAsset.Succeeded())
	{
		IA_CstmJump = JumpActionAsset.Object;
	}
	if (MoveActionAsset.Succeeded())
	{
		IA_CstmMove = MoveActionAsset.Object;
	}
	if (LookActionAsset.Succeeded())
	{
		IA_CstmLook = LookActionAsset.Object;
	}
	if (PrimaryAttackActionAsset.Succeeded())
	{
		IA_CstmPrimaryAttack = PrimaryAttackActionAsset.Object;
	}
	if (ChangeWeaponActionAsset.Succeeded())
	{
		IA_CstmChangeWeapon = ChangeWeaponActionAsset.Object;
	}
	if (SecondaryAttackActionAssets.Succeeded())
	{
		IA_CstmSecondaryAttack = SecondaryAttackActionAssets.Object;
	}
	if (ReviveActionAssets.Succeeded())
	{
		IA_CstmRevive = ReviveActionAssets.Object;
	}
	if (ToggleTrapModeActionAsset.Succeeded())
	{
		IA_CstmToggleTrapMode = ToggleTrapModeActionAsset.Object;
	}
	if (PlaceTrapActionAsset.Succeeded())
	{
		IA_CstmPlaceTrap = PlaceTrapActionAsset.Object;
	}
	if (SwapTrapActionAsset.Succeeded())
	{
		IA_CstmSwapTrap = SwapTrapActionAsset.Object;
	}
}

#pragma region MappingInterface

bool ABasePlayerController::SwitchToTrapMode()
{
	// remove combat 
	bool bCombatRemovalResult = ManageInputMappingContext(IMC_MainCharacterCombat, false);
	// add traps
	bool bTrapsAddResult = ManageInputMappingContext(IMC_MainCharacterTraps, true);
	return bCombatRemovalResult && bTrapsAddResult;
}

bool ABasePlayerController::SwitchToCombatMode()
{	
	// remove traps
	bool bTrapsAddResult = ManageInputMappingContext(IMC_MainCharacterTraps, false);
	// remove combat
	bool bCombatRemovalResult = ManageInputMappingContext(IMC_MainCharacterCombat, true);
	return bCombatRemovalResult && bTrapsAddResult;
}

#pragma endregion

#pragma region InternalInputMappingFunctions

bool ABasePlayerController::ManageInputMappingContext(const UInputMappingContext* InputMapping, bool Add)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer()))
	{
		if (IMC_MainCharacter)
		{
			if (Add)
			{
				Subsystem->AddMappingContext(InputMapping, 0);
				return true;
			}
			Subsystem->RemoveMappingContext(InputMapping);
			return true;
		}
	}
	return false;
}

#pragma endregion

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	ManageInputMappingContext(IMC_MainCharacter, true);
	ManageInputMappingContext(IMC_MainCharacterCombat, true);
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInput)
	{
		if (IA_CstmMove)
		{
			EnhancedInput->BindAction(IA_CstmMove, ETriggerEvent::Triggered, this, &ABasePlayerController::Move);
		}
		if (IA_CstmLook)
		{
			EnhancedInput->BindAction(IA_CstmLook, ETriggerEvent::Triggered, this, &ABasePlayerController::Look);
		}
		if (IA_CstmJump)
		{
			EnhancedInput->BindAction(IA_CstmJump, ETriggerEvent::Triggered, this, &ABasePlayerController::Jump);
		}
		if (IA_CstmPrimaryAttack)
		{
			EnhancedInput->BindAction(IA_CstmPrimaryAttack, ETriggerEvent::Started, this, &ABasePlayerController::PrimaryAttack);
		}
		if(IA_CstmChangeWeapon)
		{
			EnhancedInput->BindAction(IA_CstmChangeWeapon, ETriggerEvent::Started, this, &ABasePlayerController::ChangeWeapon);
		}
		if(IA_CstmSecondaryAttack)
		{
			EnhancedInput->BindAction(IA_CstmSecondaryAttack, ETriggerEvent::Triggered, this, &ABasePlayerController::SecondaryAttack);
		}
		if(IA_CstmRevive)
		{
			EnhancedInput->BindAction(IA_CstmRevive, ETriggerEvent::Started, this, &ABasePlayerController::Revive);
		}
		if (IA_CstmToggleTrapMode)
		{
			EnhancedInput->BindAction(IA_CstmToggleTrapMode, ETriggerEvent::Started, this, &ABasePlayerController::ToggleTrapMode);
		}
		if (IA_CstmSwapTrap)
		{
			EnhancedInput->BindAction(IA_CstmSwapTrap, ETriggerEvent::Started, this, &ABasePlayerController::SwapTrap);
		}
		if (IA_CstmPlaceTrap)
		{
			EnhancedInput->BindAction(IA_CstmPlaceTrap, ETriggerEvent::Started, this, &ABasePlayerController::PlaceTrap);
		}
	}
}

void ABasePlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{		
		ControlledPawn->Move_Implementation(MovementVector);
	}
}

void ABasePlayerController::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{		
		ControlledPawn->Look_Implementation(LookVector);
	}
}

void ABasePlayerController::PrimaryAttack()
{
	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{
		ControlledPawn->PrimaryAttack_Implementation();
	}
}

void ABasePlayerController::SecondaryAttack()
{
	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{
		ControlledPawn->SecondaryAttack_Implementation();
	}
}

void ABasePlayerController::Jump()
{
}

void ABasePlayerController::ChangeWeapon(const FInputActionValue& Value)
{
	float reusult = Value.Get<float>();
	bool bForward = reusult > 0 ? true : false;

	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{
		ControlledPawn->ChangeWeapon_Implementation(bForward);
	}
}

void ABasePlayerController::Revive()
{
	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{
		ControlledPawn->Revive_Implementation();
	}
}

void ABasePlayerController::ToggleTrapMode()
{
	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{
		ControlledPawn->ToggleTrapMode_Implementation();
	}
}

void ABasePlayerController::PlaceTrap()
{
	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{
		ControlledPawn->PlaceTrap_Implementation();
	}
}

void ABasePlayerController::SwapTrap(const FInputActionValue& Value)
{
	float result = Value.Get<float>();
	bool bForward = result > 0;

	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{
		ControlledPawn->SwapTrap_Implementation(bForward);
	}
}



