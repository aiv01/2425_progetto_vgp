#include "BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../Interfaces/I_PlayerInput.h"
#include "GameFramework/Character.h"

ABasePlayerController::ABasePlayerController()
{
	// Percorso degli asset nel Content Browser
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_MainCharacterAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IMC_MainCharacter.IMC_MainCharacter"));
	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmJump.IA_CstmJump"));
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmMove.IA_CstmMove"));
	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmLook.IA_CstmLook"));
	static ConstructorHelpers::FObjectFinder<UInputAction> AttackActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmAttack.IA_CstmAttack"));
	static ConstructorHelpers::FObjectFinder<UInputAction> ChangeWeaponActionAsset(TEXT("/Game/Custom/Characters/MainCharacter/Inputs/IA_CstmChangeWeapon.IA_CstmChangeWeapon"));

	// Assegna gli asset trovati
	if (IMC_MainCharacterAsset.Succeeded()) IMC_MainCharacter = IMC_MainCharacterAsset.Object;
	if (JumpActionAsset.Succeeded()) IA_CstmJump = JumpActionAsset.Object;
	if (MoveActionAsset.Succeeded()) IA_CstmMove = MoveActionAsset.Object;
	if (LookActionAsset.Succeeded()) IA_CstmLook = LookActionAsset.Object;
	if (AttackActionAsset.Succeeded()) IA_CstmAttack = AttackActionAsset.Object;
	if (ChangeWeaponActionAsset.Succeeded()) IA_CstmChangeWeapon = ChangeWeaponActionAsset.Object;
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
		if (IA_CstmMove) EnhancedInput->BindAction(IA_CstmMove, ETriggerEvent::Triggered, this, &ABasePlayerController::Move);
		if (IA_CstmLook) EnhancedInput->BindAction(IA_CstmLook, ETriggerEvent::Triggered, this, &ABasePlayerController::Look);
		if (IA_CstmJump) EnhancedInput->BindAction(IA_CstmJump, ETriggerEvent::Triggered, this, &ABasePlayerController::Jump);
		if (IA_CstmAttack)
		{
			EnhancedInput->BindAction(IA_CstmAttack, ETriggerEvent::Started, this, &ABasePlayerController::Attack);
		}
		if(IA_CstmChangeWeapon)
		{
			EnhancedInput->BindAction(IA_CstmChangeWeapon, ETriggerEvent::Started, this, &ABasePlayerController::ChangeWeapon);
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

void ABasePlayerController::Attack()
{
	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack chiamato!")); // <-- Log per debug
		ControlledPawn->Attack_Implementation();
	}
}

void ABasePlayerController::Jump()
{
	// if (ACharacter* Character = Cast<ACharacter>(GetPawn()))
	// {
	// 	Character->Jump();
	// }
}

void ABasePlayerController::ChangeWeapon(const FInputActionValue& Value)
{
	float reusult = Value.Get<float>();
	bool bForward = reusult > 0 ? true : false;
	

	if (II_PlayerInput* ControlledPawn = Cast<II_PlayerInput>(GetPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ChangeWeapon chiamato: %hhd"), bForward);
		ControlledPawn->ChangeWeapon_Implementation(bForward);
	}
}

