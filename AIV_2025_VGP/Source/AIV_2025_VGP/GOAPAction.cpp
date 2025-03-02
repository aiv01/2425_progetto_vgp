// Fill out your copyright notice in the Description page of Project Settings.
// Marco Pungillo

#include "GOAPWorldModel.h"
#include "GOAPAction.h"

bool UGOAPAction::IsFeasable(UGOAPWorldModel* Wm)
{
	bool Feasibility = true;
	for (TPair condition : PreConditions)
	{
		if (Wm->Model.ConditionsModel.Contains(condition.Key))
		{
			Feasibility = (Wm->Model.ConditionsModel[condition.Key] == condition.Value);
		}
	}

	return Feasibility;
}

float UGOAPAction::GetGoalChange(const FGOAPGoal goal) const
{
	float total = 0;

	if (Satisfiers.Contains(goal.Name))
	{
		const float* valueptr = Satisfiers.Find(goal.Name);
		if (valueptr)
		{
			total += *valueptr;
		}
	}

	return total;
}
