// Fill out your copyright notice in the Description page of Project Settings.

#include "GOAPAction.h"
#include "GOAPAgentComponent.h"
#include "GOAPWorldModel.h"

float UGOAPWorldModel::CalculateDiscontentment()
{
	float discontentment = 0;
	for (FGOAPGoal Goal : Model.Insistences)
	{
		discontentment += Goal.GetDiscontentment(Goal.Value);
	}
	return discontentment;

}

UGOAPAction* UGOAPWorldModel::NextAction()
{
	UGOAPAction* returnValue = nullptr;

	if (CurrentActionIndex < UsableActions.Num())
	{
		returnValue = UsableActions[CurrentActionIndex];
	}
	CurrentActionIndex++;

	return returnValue;
}

void UGOAPWorldModel::ApplyAction(const UGOAPAction* Action)
{
	CurrentActionIndex = 0;
	for (FGOAPGoal& Goal : Model.Insistences)
	{
		if(Action->Satisfiers.Contains(Goal.Name))
		{
			Goal.Value += Action->GetGoalChange(Goal);
			Goal.Value = FMath::Clamp(Goal.Value, 0, Model.MaxParameterInsistence);

		}
	}
}
