// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EIPlayerHUDWidget.h"
#include "UI/EIStatBarWidget.h"
#include "Components/EIAttributeComponent.h"

UEIPlayerHUDWidget::UEIPlayerHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UEIPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UEIAttributeComponent* AttributeComp = Pawn->GetComponentByClass<UEIAttributeComponent>())
		{
			AttributeComp->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
			AttributeComp->BroadcastAttributeChanged(EIAttributeType::Stamina);
		}
	}
}

void UEIPlayerHUDWidget::OnAttributeChanged(EIAttributeType AttributeType, float InValue)
{
	switch (AttributeType)
	{
	case EIAttributeType::Stamina:
		StaminaBarWidget->SetRatio(InValue);
		break;
	case EIAttributeType::Health:
		break;
	default:
		break;
	}
}
