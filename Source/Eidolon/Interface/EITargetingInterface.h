// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EITargetingInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEITargetingInterface : public UInterface
{
	GENERATED_BODY()
};

class EIDOLON_API IEITargetingInterface
{
	GENERATED_BODY()

public:
	virtual void OnTargeted(bool bTargeted) = 0;
	virtual bool CanBeTargeted() = 0;
};
