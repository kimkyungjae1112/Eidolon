// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EIAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class EIDOLON_API UEIAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
	TObjectPtr<ACharacter> Character;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
	TObjectPtr<UCharacterMovementComponent> MovementComp;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	uint8 bIsIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	float Direction;

public:
	UEIAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UFUNCTION()
	void AnimNotify_ResetMovementInput();

	UFUNCTION()
	void AnimNotify_ResetState();
};
