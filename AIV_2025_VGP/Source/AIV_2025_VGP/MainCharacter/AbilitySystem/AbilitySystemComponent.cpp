// Copyright Epic Games, Inc. All Rights Reserved.
// Fabio Giannino
// Fabrizio Conni
// Luca Casamenti

#include "AbilitySystemComponent.h"
//DEFINE_LOG_CATEGORY_STATIC(LogStrategy, Log, All);
// Sets default values for this component's properties
UAbilitySystemComponent::UAbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAbilitySystemComponent::ExecuteStrategy(FName StrategyName)
{
	if(StrategyInstances.Contains(StrategyName))
	{
		UBaseStrategy* StrategyInstance = StrategyInstances[StrategyName];
		
		if (StrategyInstance && StrategyInstance->Implements<UI_FunctionStrategy>())
		{
			II_FunctionStrategy* StrategyInterface = Cast<II_FunctionStrategy>(StrategyInstance);
			if (StrategyInterface)
			{
				//UE_LOG(LogStrategy, Log, TEXT("StrategyComponent: Eseguendo la strategia %s"), *StrategyInstance->GetName());
				StrategyInterface->Execute();
			}
		}
		else
		{
			//UE_LOG(LogStrategy, Error, TEXT("StrategyComponent: Nessuna strategia valida impostata!"));
		}
	}
	
}

UBaseStrategy* UAbilitySystemComponent::ClassTypeGetExecutedStrategy(FName StrategyName)
{
	if (StrategyInstances.Contains(StrategyName))
	{
		UBaseStrategy* StrategyInstance = StrategyInstances[StrategyName];

		if (StrategyInstance)
		{
			return StrategyInstance;
		}
	}

	return nullptr;
}

bool UAbilitySystemComponent::GetStrategyParamGeneric(FName StrategyName, const FString& ParamName, FString& OutValue)
{
	if (StrategyInstances.Contains(StrategyName))
	{
		UBaseStrategy* StrategyInstance = StrategyInstances[StrategyName];

		if (StrategyInstance)
		{
			FProperty* Property = StrategyInstance->GetClass()->FindPropertyByName(*ParamName);
			if (Property)
			{
				void* PropertyAddr = Property->ContainerPtrToValuePtr<void>(StrategyInstance);

				if (FNumericProperty* NumProp = CastField<FNumericProperty>(Property))
				{
					OutValue = FString::SanitizeFloat(NumProp->GetFloatingPointPropertyValue(PropertyAddr));
					return true;
				}
				else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
				{
					OutValue = BoolProp->GetPropertyValue(PropertyAddr) ? TEXT("True") : TEXT("False");
					return true;
				}
				else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
				{
					OutValue = *StrProp->GetPropertyValue(PropertyAddr);
					return true;
				}
				else if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
				{
					OutValue = NameProp->GetPropertyValue(PropertyAddr).ToString();
					return true;
				}
			}
		}
	}

	return false;
}