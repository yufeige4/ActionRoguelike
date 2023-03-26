// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GBTService_HealthCheck.h"
#include "AIController.h"
#include "GBlueprintFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/GAttributeComponent.h"
#include "Kismet/GameplayStatics.h"

UGBTService_HealthCheck::UGBTService_HealthCheck(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	bInCoolDown = false;
	TimePassed = 0.0f;
}

void UGBTService_HealthCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0,DeltaSeconds,FColor::Red,FString::SanitizeFloat(TimePassed));
	}
	// 检查血量
	auto AIC = Cast<AAIController>(OwnerComp.GetOwner());
	auto MyBB = OwnerComp.GetBlackboardComponent();
	if(AIC)
	{
		APawn* ControlledPawn = AIC->GetPawn();
		auto AttributeComp = UGAttributeComponent::GetAttributeComponent(ControlledPawn);
		if(AttributeComp)
		{
			float HPRatio = AttributeComp->GetCurrHealth()/AttributeComp->GetMaxHealth();
			float FULLHEALTH = 1.0f;
			if(!bInCoolDown)
			{
				if(HPRatio < EscapeRatio)
				{
					MyBB->SetValueAsEnum(Key_AIState.SelectedKeyName,static_cast<uint8>(EAIState::OutCombat));
					bInCoolDown = true;
					TimePassed = 0.0f;
				}
			}else
			{
				
				TimePassed += DeltaSeconds;
				// Reset cooldown
				if(TimePassed>=RecoverAbilityCD)
				{
					TimePassed = 0.0f;
					bInCoolDown = false;
				}
				// Back to battle
				if(HPRatio==FULLHEALTH && MyBB->GetValueAsEnum(Key_AIState.SelectedKeyName)==static_cast<uint8>(EAIState::OutCombat))
				{
					MyBB->SetValueAsEnum(Key_AIState.SelectedKeyName,static_cast<uint8>(EAIState::InCombat));
					UE_LOG(LogTemp,Log,TEXT("Back to battle!!!"));
				}
			}
		}
	}
}


