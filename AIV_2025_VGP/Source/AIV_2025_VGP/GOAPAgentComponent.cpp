// Fill out your copyright notice in the Description page of Project Settings.
// Marco Pungillo
#include "GOAPAction.h"
#include "GOAPWorldModel.h"
#include "GOAPAgentComponent.h"

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

UGOAPAction* UGOAPAgentComponent::ChooseAction()
{
#pragma region FirstImplementation
	//FGOAPGoal TopGoal = Goals[0];
	//for (FGOAPGoal goal : Goals)
	//{
	//	// Search for the current most important goal
	//	if (goal.Value > TopGoal.Value)
	//	{
	//		TopGoal = goal;
	//	}
	//}

	//// Search for the best action to take
	//UGOAPAction* bestAction = Actions[0];
	//float bestUtility = -(bestAction->GetGoalChange(TopGoal));

	//for (UGOAPAction* action : Actions)
	//{
	//	// We are inverting the change beacuase a low change value is good
	//	// But we want to operate with greater = better
	//	float utility = -(action->GetGoalChange(TopGoal));

	//	if (utility > bestUtility)
	//	{
	//		bestUtility = utility;
	//		bestAction = action;
	//	}
	//}

	//// Return the best action to be carried out
	//return bestAction;
#pragma endregion

#pragma region SecondImplementation
	
	UGOAPAction* bestAction = nullptr;
	float bestValue = INFINITY;

	for (UGOAPAction* act : Actions)
	{
		float value = Discontentment(act);
		if (value < bestValue)
		{
			bestValue = value;
			bestAction = act;
		}
	}

	return bestAction;
#pragma endregion
}

float UGOAPAgentComponent::Discontentment(const UGOAPAction* Action)
{
	float DiscontentementValue = 0;

	for (FGOAPGoal goal : Goals)
	{
		// Calculate the new value after the action
		float newValue = goal.Value + Action->GetGoalChange(goal);

		// Get the discontentment of this value
		DiscontentementValue += goal.GetDiscontentment(newValue);
	}
	
	return DiscontentementValue;
}

UGOAPAction* UGOAPAgentComponent::PlanAction(UGOAPWorldModel* wdModel, const int maxDepth)
{
	TArray<UGOAPWorldModel*> Models;
	Models.SetNum(maxDepth + 1);
	TArray<UGOAPAction*> ActionsToUse;
	ActionsToUse.SetNum(maxDepth);

	// Setup datas
	wdModel->UsableActions = Actions;
	wdModel->CurrentActionIndex = 0;
	Models[0] = wdModel;
	int CurrentDepth = 0;

	UGOAPAction* BestAction = nullptr;
	float BestValue = INFINITY;

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
			}
			
			CurrentDepth -= 1;
		}
		// Otherwise we need to try the next action
		else
		{
			UGOAPAction* NextAction = Models[CurrentDepth]->NextAction();
			if (NextAction)
			{
				// We have an action to apply, copy the current model
				Models[CurrentDepth+1] = DuplicateObject(Models[CurrentDepth], GetTransientPackage());

				// Apply the action to the copy
				ActionsToUse[CurrentDepth] = NextAction;
				Models[CurrentDepth + 1]->ApplyAction(NextAction);

				// process it on the next iteration
				CurrentDepth += 1;
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