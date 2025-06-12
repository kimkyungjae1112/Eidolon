// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EIInteractInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEIInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class EIDOLON_API IEIInteractInterface
{
	GENERATED_BODY()

public:
	virtual void Interact(AActor* Interactor) = 0;
};
