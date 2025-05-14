// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLanded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeHealth, AActor*, Actor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEALTHSYSTEM_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", ReplicatedUsing=OnRep_Health)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bCanRevive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float LandedTimer;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_Health(float OldHealth);

	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangeHealth(float Delta);

	void ServerChangeHealth(float Delta);

	UFUNCTION()
	void OnLandedCallback();
	UFUNCTION()
	void OnDeathCallback();
	UFUNCTION()
	void OnChangeHealthCallback(AActor* Actor);

	FTimerHandle DeathTimerHandle;
public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ChangeHealth(float Delta);
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetPercentHealth() const;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Health")
	FOnLanded OnLanded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Health")
	FOnDeath OnDeath;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Health")
	FOnChangeHealth OnChangeHealth;

#pragma region Revive System
private:
	UPROPERTY(Replicated)
	bool bCanReviveFriend = false;	

	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangeReviveFriendStatus(bool bNewStatus, AActor* NewActor);

	void ServerChangeReviveFriendStatus(bool bNewStatus, AActor* NewActor);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	AActor* ActorToRevive = nullptr;

	UFUNCTION(BlueprintCallable, Category="Health")
	void ChangeReviveFriendStatus(bool bNewStatus, AActor* NewActor);
#pragma endregion
};
