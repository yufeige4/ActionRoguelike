// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPlayerStateBase.h"

#include "AI/GAICharacter.h"
#include "Components/GEventManager.h"
// @fix-me: improve by getting it from data table
#define Credit_KILLMONSTER 10
#define Credit_USEPOTION -5
#define Credit_PICKUPCOIN 1

AGPlayerStateBase::AGPlayerStateBase()
{
	Credit = 0;
}

bool AGPlayerStateBase::ApplyCreditChange(int Delta)
{
	int PrevCredit = Credit;
	int NewCredit = FMath::Clamp(PrevCredit+Delta,0,INT_MAX);
	int ActualDelta = NewCredit - PrevCredit;
	if(ActualDelta==0)
	{
		return false;
	}
	Credit = NewCredit;
	OnCreditChange.Broadcast(ActualDelta,PrevCredit,NewCredit);
	return true;
}

void AGPlayerStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	auto EventManger = UGEventManager::GetEventManager(GetOwner());
	if(ensureMsgf(EventManger,TEXT("EventManger not exist"))){
		EventManger->OnKillActor.AddDynamic(this,&AGPlayerStateBase::CreditChangeOnKillEvent);
		EventManger->OnUseItem.AddDynamic(this,&AGPlayerStateBase::CreditChangeOnUseItemEvent);
		EventManger->OnPickUpItem.AddDynamic(this,&AGPlayerStateBase::CreditChangeOnPickUpItemEvent);
	}
}

void AGPlayerStateBase::CreditChangeOnKillEvent(AActor* Victim, AActor* Killer)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	AActor* ControlledActor = nullptr;
	if(ensure(PC))
	{
		ControlledActor = PC->GetPawn();
	}
	if(Killer==ControlledActor)
	{
		// @fix-me: this logic need to be replaced by proper monster deciding logic after learning gameplay tag
		ApplyCreditChange(Credit_KILLMONSTER);
	}
}

void AGPlayerStateBase::CreditChangeOnUseItemEvent(AActor* User, AActor* Item)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	AActor* ControlledActor = nullptr;
	if(ensure(PC))
	{
		ControlledActor = PC->GetPawn();
	}
	if(User==ControlledActor)
	{
		// @fix-me: need to read from data table
		ApplyCreditChange(Credit_USEPOTION);
	}
}

void AGPlayerStateBase::CreditChangeOnPickUpItemEvent(AActor* Picker, AActor* Item)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	AActor* ControlledActor = nullptr;
	if(ensure(PC))
	{
		ControlledActor = PC->GetPawn();
	}
	if(Picker==ControlledActor)
	{
		// @fix-me: need to read from data table
		ApplyCreditChange(Credit_PICKUPCOIN);
	}
}