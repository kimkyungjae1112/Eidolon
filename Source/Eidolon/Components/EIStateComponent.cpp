// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EIStateComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UEIStateComponent::UEIStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UEIStateComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UEIStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UEIStateComponent::ToggleMovementInput(bool bEnabled, float Duration)
{
	if (bEnabled)
	{
		FLatentActionInfo LatentAction;
		LatentAction.CallbackTarget = this;
		LatentAction.ExecutionFunction = "MovementInputEnableAction";
		LatentAction.Linkage = 0;
		LatentAction.UUID = 0;
		
		// 먼저 들어온 요청이 처리되기 전 다시 요청이 들어올 때, 그 전 요청은 무시한다.
		UKismetSystemLibrary::RetriggerableDelay(GetWorld(), Duration, LatentAction);
	}
	else
	{
		bMovementInputEnabled = false;
	}
}

void UEIStateComponent::MovementInputEnableAction()
{
	bMovementInputEnabled = true;
	ClearState();
}

void UEIStateComponent::ClearState()
{
	CurrentState = FGameplayTag::EmptyTag;
}

bool UEIStateComponent::IsCurrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const
{
	return TagsToCheck.HasTagExact(CurrentState);
}

