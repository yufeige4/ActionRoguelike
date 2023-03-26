// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GUserWidget_World.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"

void UGUserWidget_World::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 判断该Actor是否被回收或即将被回收
	if(!IsValid(AttachedActor))
	{
		// 销毁自身实例
		RemoveFromParent();
		UE_LOG(LogTemp,Warning,TEXT("AttachedActor is destroyed, removing related UserWidget"));
		return;
	}
	
	FVector2D ScreenPosition;
	bool bProjectSuccess = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(),AttachedActor->GetActorLocation() + WorldDisplayOffset,ScreenPosition);
	if(bProjectSuccess)
	{
		float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= ViewportScale;

		if(ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}
}
