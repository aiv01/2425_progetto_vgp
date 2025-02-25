// Fill out your copyright notice in the Description page of Project Settings.

#include "GOAPAction.h"
//#include "GOAPWorldModel.h"
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
	float discontentementValue = 0;

	for (FGOAPGoal goal : Goals)
	{
		// Calculate the new value after the action
		float newValue = goal.Value + Action->GetGoalChange(goal);

		// Get the discontentment of this value
		discontentementValue += goal.GetDiscontentment(newValue);
	}
	
	return discontentementValue;
}


//UGOAPAction* UGOAPAgentComponent::PlanAction(UGOAPWorldModel wdModel, const int maxDepth)
//{
//	TArray<UGOAPWorldModel> models;
//	TArray<UGOAPAction*> actions;
//
//	// Setting up the initial datas
//	models.Insert(wdModel, 0);
//	int currentDepth = 0;
//
//	UGOAPAction* bestAction = nullptr;
//	float bestValue = INFINITY;
//
//	// Iteration of all action at depth zero
//	while (currentDepth >= 0)
//	{
//		// Are we on maximum search depth?
//		if (currentDepth >= maxDepth)
//		{
//			// Calculate discontentment at the deepest level
//			float currentValue = models[currentDepth].calculateDiscontentment();
//
//			// If the current value is the best, store the first step of the actions path
//			if (currentValue < bestValue)
//			{
//				bestValue = currentValue;
//				bestAction = actions[0];
//			}
//
//			// Depth completed, drop back
//			currentDepth -= 1;
//		}
//		else // otherwise, we need to try the next action
//		{
//			UGOAPAction* nextAction = models[currentDepth].NextAction();
//			if (nextAction)
//			{
//				// We have an action to Apply, so we copy the current model
//				models.Insert(models[currentDepth], currentDepth + 1);
//
//				// Apply the action to the copy
//				actions.Insert(nextAction, currentDepth);
//				models[currentDepth+1].ApplyAction(nextAction);
//
//				// Go to next iteration
//				currentDepth += 1;
//			}
//			// Otherwise we have no action to try, so we're done at this level
//			else
//			{
//				// drop back to the next highest level
//				currentDepth -= 1;
//			}
//		}
//	}
//
//
//	return nullptr;
//}

