// Fill out your copyright notice in the Description page of Project Settings.

#include "GOAPAction.h"
#include "GOAPWorldModel.h"

float UGOAPWorldModel::calculateDiscontentment()
{
	return 0.0f;
}

UGOAPAction* UGOAPWorldModel::NextAction()
{
	return nullptr;
}

void UGOAPWorldModel::ApplyAction(const UGOAPAction* action)
{
}
