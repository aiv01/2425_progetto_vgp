// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAction.h"

float UGOAPAction::GetGoalChange(const FGOAPGoal goal)
{
	float total = 0;

	if (Satisfiers.Contains(goal.Name))
	{
		float* valueptr = Satisfiers.Find(goal.Name);
		if (valueptr)
		{
			total += *valueptr;
		}
	}

	return total;
}
