// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EIStatBarWidget.h"
#include "Components/ProgressBar.h"

UEIStatBarWidget::UEIStatBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UEIStatBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (StatBar)
	{
		StatBar->SetFillColorAndOpacity(FillColorAndOpacity);
	}
}

void UEIStatBarWidget::SetRatio(float Ratio) const
{
	if (StatBar)
	{
		StatBar->SetPercent(Ratio);
	}
}