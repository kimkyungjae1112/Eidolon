// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EIDefine.h"
#include "Blueprint/UserWidget.h"
#include "EIPlayerHUDWidget.generated.h"

class UEIStatBarWidget;

UCLASS()
class EIDOLON_API UEIPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UEIStatBarWidget> StaminaBarWidget;

public:
	UEIPlayerHUDWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct() override;

protected:
	void OnAttributeChanged(EIAttributeType AttributeType, float InValue);
};
