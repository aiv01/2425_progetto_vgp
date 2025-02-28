// Fill out your copyright notice in the Description page of Project Settings.
// Marco Pungillo

#include "GOAPAction.h"

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
