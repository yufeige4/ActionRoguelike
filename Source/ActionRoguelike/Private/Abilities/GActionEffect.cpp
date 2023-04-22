// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GActionEffect.h"
#include "GGameplayFunctionLibrary.h"
#include "Components/GActionComponent.h"

void UGActionEffect::ExecutePeriodEffect_Implementation(AActor* Instigator)
{
	if(DebugActionTag.GetValueOnGameThread() && GEngine)
	{
		FString DebugMsg = GetNameSafe(this) + "'s periodical effect on " + GetNameSafe(GetOwningActionComp()->GetOwner())  +" is triggered!";
		GEngine->AddOnScreenDebugMessage(-1,Duration,FColor::Green,DebugMsg);
	}
}

UGActionEffect::UGActionEffect()
{
	bAutoStart = true;
}

void UGActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if(Duration>0.0f)
	{
		FTimerDelegate DurationDelegate;
		DurationDelegate.BindUFunction(this,"StopAction",Instigator);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration,DurationDelegate,Duration,false);
	}
	if(Period>0.0f)
	{
		FTimerDelegate PeriodDelegate;
		PeriodDelegate.BindUFunction(this,"ExecutePeriodEffect",Instigator);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Period,PeriodDelegate,Period,true);
	}
}

void UGActionEffect::StopAction_Implementation(AActor* Instigator)
{
	// should not miss the last tick trigger of period event
	if(GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Period)< KINDA_SMALL_NUMBER)
	{
		ExecutePeriodEffect(Instigator);
	}
	
	Super::StopAction_Implementation(Instigator);
	// stop all the timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Duration);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Period);

	UGActionComponent* ActionComp = GetOwningActionComp();
	if(ActionComp)
	{
		ActionComp->RemoveAction(this);
	}
}
