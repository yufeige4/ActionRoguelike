// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GGameModeBase.h"

AGGameModeBase::AGGameModeBase()
{
	SpawnControlComp = CreateDefaultSubobject<UGSpawnControlComponent>("SpawnControlComp");
}

void AGGameModeBase::StartPlay()
{
	Super::StartPlay();
	SpawnControlComp->SpawnObjects();
}


