// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EIAttributeComponent.h"
#include "EIStateComponent.h"
#include "EIGameplayTags.h"

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

void UEIAttributeComponent::TakeDamageAmount(float DamageAmount)
{
	// 체력 차감
	BaseHealth = FMath::Clamp(BaseHealth - DamageAmount, 0.f, MaxHealth);

	BroadcastAttributeChanged(EIAttributeType::Health);

	if (BaseHealth <= 0.f)
	{
		// Call Death Delegate
		if (OnDeath.IsBound())
		{
			OnDeath.Broadcast();
		}

		// Set Death State
		if (UEIStateComponent* StateComp = GetOwner()->GetComponentByClass<UEIStateComponent>())
		{
			StateComp->SetState(EIGameplayTags::Character_State_Death);
		}
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

