// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#include "BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../Interfaces/I_PlayerInput.h"
#include "Kismet/GameplayStatics.h"

ABasePlayerController::ABasePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FindInputActions();
}

TArray<ABasePlayer*> ABasePlayerController::GetAllBasePlayers() const
{
		TArray<AActor*> TempActor;
		TArray<ABasePlayer*> BasePlayers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlayer::StaticClass(), TempActor);
		for (auto Actor : TempActor)
		{
			if(Actor == GetOwner())
			{
				continue;
			}
			// Cast the actor to ABasePlayer and add it to the array
			BasePlayers.Add(Cast<ABasePlayer>(Actor));
		}
		return BasePlayers;
}

void ABasePlayerController::FindInputActions()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_MainCharacterAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IMC_MainCharacter.IMC_MainCharacter"));
	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmJump.IA_CstmJump"));
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmMove.IA_CstmMove"));
	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmLook.IA_CstmLook"));
	static ConstructorHelpers::FObjectFinder<UInputAction> PrimaryAttackActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmPrimaryAttack.IA_CstmPrimaryAttack"));
	static ConstructorHelpers::FObjectFinder<UInputAction> ChangeWeaponActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmChangeWeapon.IA_CstmChangeWeapon"));
	static ConstructorHelpers::FObjectFinder<UInputAction> SecondaryAttackActionAssets(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmSecondaryAttack.IA_CstmSecondaryAttack"));
	
	if (IMC_MainCharacterAsset.Succeeded())
	{
		IMC_MainCharacter = IMC_MainCharacterAsset.Object;		
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
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(this))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (IMC_MainCharacter)
			{
				Subsystem->AddMappingContext(IMC_MainCharacter, 0);
			}
		}
	}
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



