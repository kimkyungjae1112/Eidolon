// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EIStatBarWidget.generated.h"

class UProgressBar;

UCLASS()
class EIDOLON_API UEIStatBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UProgressBar> StatBar;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "StatBar")
	FLinearColor FillColorAndOpacity = FLinearColor(0.f, 0.9f, 1.f, 1.f);

public:
	UEIStatBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativePreConstruct() override;

public:
	void SetRatio(float Ratio) const;

};
