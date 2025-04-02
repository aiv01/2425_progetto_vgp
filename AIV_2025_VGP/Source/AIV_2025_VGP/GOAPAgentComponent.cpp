// Fill out your copyright notice in the Description page of Project Settings.
// COPYRIGHT
// Marco Pungillo
// Alessandro Violante

#include "GOAPAgentComponent.h"
#include "GOAPAction.h"
#include "GOAPGoalDefiner.h"
#include "GOAPWorldModel.h"

// Sets default values for this component's properties
UGOAPAgentComponent::UGOAPAgentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGOAPAgentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGOAPAgentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UGOAPAction* UGOAPAgentComponent::PlanAction(UGOAPWorldModel* wdModel, const int maxDepth)
{
	// Models and Actions Array Initialization 
	TArray<UGOAPWorldModel*> Models;
	Models.SetNum(maxDepth + 1);
	TArray<UGOAPAction*> ActionsToUse;
	ActionsToUse.SetNum(maxDepth);

	// Setup datas
	wdModel->UsableActions = Actions;
	wdModel->CurrentActionIndex = 0;
	Models[0] = wdModel;
	int CurrentDepth = 0;

	// Setup of the comparison variables
	UGOAPAction* BestAction = nullptr;
	float BestValue = INFINITY;
	UGOAPWorldModel* BestStartingModel = nullptr;

	// Iteration of the zero-depth actions
	while (CurrentDepth >= 0)
	{
		if (CurrentDepth == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Sul primo livello"));
		}
		//Check if we are at maximum depth
		if (CurrentDepth >= maxDepth)
		{
			// Discontentment calculation at deepest level
			float currentValue = Models[CurrentDepth]->CalculateDiscontentment();

			if (currentValue < BestValue)
			{
				BestValue = currentValue;
				BestAction = ActionsToUse[0];
				BestStartingModel = Models[1];
			}
			else if (currentValue == BestValue)
			{
				float currentDiscontenment = Models[1]->CalculateDiscontentment();
				float oldDiscontenment = BestStartingModel->CalculateDiscontentment();
				if (currentDiscontenment < oldDiscontenment)
				{
					BestValue = currentValue;
					BestAction = ActionsToUse[0];
					BestStartingModel = Models[1];
				}
			}
			
			CurrentDepth -= 1;
		}
		// Otherwise we need to try the next action
		else
		{

			UGOAPAction* NextAction = Models[CurrentDepth]->NextAction();

			if (NextAction)
			{
				if (NextAction->IsFeasable(Models[CurrentDepth]))
				{
					if (NextAction->EvaluateFeasibility(Models[CurrentDepth]))
					{
						// We have an action to apply, copy the current model
						Models[CurrentDepth + 1] = DuplicateObject(Models[CurrentDepth], GetTransientPackage());

						// Apply the action to the copy
						ActionsToUse[CurrentDepth] = NextAction;
						Models[CurrentDepth + 1]->ApplyAction(NextAction);

						// process it on the next iteration
						CurrentDepth += 1;
					}
				}
			}

			// Otherwise we have no action to try, so we're done for this level
			else 
			{
				CurrentDepth -= 1;
			}
		}

	}

	return BestAction;
}