// Fill out your copyright notice in the Description page of Project Settings.
// Marco Pungillo
// Andreea Hodor
// Alessandro Violante


#pragma once

#include "CoreMinimal.h"
#include "I_CombatAgent.h"
#include "I_MovingAgent.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UCLASS()
class AIV_2025_VGP_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//// Agent Interfaces
	//	// Combat Agent
	//virtual void I_ExecuteSkill(const FName SkillName) override;
	//virtual void I_EndSkill(const FName SkillName) override;

	//	// Moving Agent
	//virtual float I_GetSpeed() override;
	//virtual void I_SetSpeed(const float NewSpeed) override;
};
