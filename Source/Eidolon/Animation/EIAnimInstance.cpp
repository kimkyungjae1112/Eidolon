// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/EIAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/EICharacter.h"
#include "Components/EIStateComponent.h"

UEIAnimInstance::UEIAnimInstance()
{
}

void UEIAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacter>(GetOwningActor());
	if (Character)
	{
		MovementComp = Character->GetCharacterMovement();
	}
}

void UEIAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character == nullptr || MovementComp == nullptr) return;

	Velocity = Character->GetVelocity();
	GroundSpeed = Velocity.Size2D();

	bIsIdle = GroundSpeed < 3.f && MovementComp->GetCurrentAcceleration() == FVector::ZeroVector;
	bIsFalling = MovementComp->IsFalling();
}

void UEIAnimInstance::AnimNotify_ResetMovementInput()
{
	if (AEICharacter* LocalCharacter = Cast<AEICharacter>(GetOwningActor()))
	{
		LocalCharacter->GetStateComponent()->ToggleMovementInput(true);
	}
}

void UEIAnimInstance::AnimNotify_ResetState()
{
	if (AEICharacter* LocalCharacter = Cast<AEICharacter>(GetOwningActor()))
	{
		LocalCharacter->GetStateComponent()->ClearState();
	}
}
