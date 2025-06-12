// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EIAttributeComponent.h"

UEIAttributeComponent::UEIAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UEIAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UEIAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UEIAttributeComponent::CheckHasEnoughStamina(float StaminaCost) const
{
	return BaseStamina >= StaminaCost;
}

void UEIAttributeComponent::DecreaseStamina(float StaminaCost)
{
	BaseStamina = FMath::Clamp(BaseStamina - StaminaCost, 0.0f, MaxStamina);

	BroadcastAttributeChanged(EIAttributeType::Stamina);
}

void UEIAttributeComponent::ToggleStaminaRegeneration(bool bEnabled, float StartDelay)
{
	if (bEnabled)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &ThisClass::RegenerateStaminaHandler, 0.1f, true, StartDelay);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
	}
}

void UEIAttributeComponent::BroadcastAttributeChanged(const EIAttributeType& InAttributeType) const
{
	float Ratio = 0.f;
	if(OnAttributeChanged.IsBound())
	{
		switch(InAttributeType)
		{
			case EIAttributeType::Stamina:
				Ratio = GetStaminaRatio();
				break;
			case EIAttributeType::Health:
				break;
			default:
				break;
		}

		OnAttributeChanged.Broadcast(InAttributeType, Ratio);
	}
}

void UEIAttributeComponent::RegenerateStaminaHandler()
{
	BaseStamina = FMath::Clamp(BaseStamina + StaminaRegenRate, 0.0f, MaxStamina);

	BroadcastAttributeChanged(EIAttributeType::Stamina);

	if(BaseStamina >= MaxStamina)
	{
		ToggleStaminaRegeneration(false);
	}
	
}

