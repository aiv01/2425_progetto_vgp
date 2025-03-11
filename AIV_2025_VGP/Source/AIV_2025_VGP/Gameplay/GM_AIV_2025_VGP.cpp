// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_AIV_2025_VGP.h"

AGM_AIV_2025_VGP::AGM_AIV_2025_VGP()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Custom/Characters/BP_MainCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
