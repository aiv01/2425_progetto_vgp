// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#include "BasePlayer.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "../Public/HealthSystem/I_HealthSystem.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ABasePlayer

ABasePlayer::ABasePlayer()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//set skeletal mesh
	FString SkeletalMeshPath = "/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny";
	USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *SkeletalMeshPath);
	if (SkeletalMesh)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMesh);
	}
}

void ABasePlayer::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ABasePlayer::Move_Implementation(const FVector2D& Value)
{
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Value.Y);
		AddMovementInput(RightDirection, Value.X);
	}
}

void ABasePlayer::Look_Implementation(const FVector2D& Value)
{
	
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		SetActorRotation(YawRotation);
		// add yaw and pitch input to controller
		AddControllerYawInput(Value.X);
		AddControllerPitchInput(Value.Y);
	}
}

void ABasePlayer::PrimaryAttack_Implementation()
{
	if (GetClass()->ImplementsInterface(UI_PlayerInput::StaticClass()))
	{
		II_PlayerInput::Execute_PrimaryAttack(this);
	}
}

void ABasePlayer::SecondaryAttack_Implementation()
{
	if (GetClass()->ImplementsInterface(UI_PlayerInput::StaticClass()))
	{
		II_PlayerInput::Execute_SecondaryAttack(this);
	}
}

void ABasePlayer::ChangeWeapon_Implementation(bool bForward)
{	
	if (GetClass()->ImplementsInterface(UI_PlayerInput::StaticClass()))
	{
		II_PlayerInput::Execute_ChangeWeapon(this, bForward);
	}
}

void ABasePlayer::AddHealth_Implementation(float Amount)
{
	if (GetClass()->ImplementsInterface(UI_HealthSystem::StaticClass()))
	{
		II_HealthSystem::Execute_AddHealth(this, Amount);
	}
}
void ABasePlayer::RemoveHealth_Implementation(float Amount)
{
	if (GetClass()->ImplementsInterface(UI_HealthSystem::StaticClass()))
	{
		II_HealthSystem::Execute_RemoveHealth(this, Amount);
	}
}


