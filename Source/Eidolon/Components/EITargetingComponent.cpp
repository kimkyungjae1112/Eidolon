// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EITargetingComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EIDefine.h"
#include "Interface/EITargetingInterface.h"
#include "Kismet/KismetMathLibrary.h"

UEITargetingComponent::UEITargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UEITargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	
	Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		Camera = Character->GetComponentByClass<UCameraComponent>();
	}
}


void UEITargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsLockOn == false)
	{
		return;
	}

	if (IsValid(Character) == false)
	{
		return;
	}

	if (IsValid(LockedTargetActor) == false)
	{
		return;
	}

	const float Distance = FVector::Dist(Character->GetActorLocation(), LockedTargetActor->GetActorLocation());

	if (IEITargetingInterface* Targeting = Cast<IEITargetingInterface>(LockedTargetActor))
	{
		// TargetingRadius 보다 떨어지면 LockedOn을 중단한다.
		if (Targeting->CanBeTargeted() == false || Distance > TargetingRadius)
		{
			StopLockOn();
		}
		else
		{
			FaceLockOnActor();
		}
	}

}

void UEITargetingComponent::ToggleLockOn()
{
	if (bIsLockOn)
	{
		StopLockOn();
	}
	else
	{
		LockOnTarget();
	}
}

void UEITargetingComponent::SwitchingLockedOnActor(ESwitchingDirection InDirection)
{
	if (IsValid(LockedTargetActor))
	{
		if (IEITargetingInterface* Targeting = Cast<IEITargetingInterface>(LockedTargetActor))
		{
			// 기존 타겟 해제
			Targeting->OnTargeted(false);

			// 신규 타겟 지정
			TArray<AActor*> OutTargets;
			FindTargets(OutTargets);
			AActor* TargetActor = FindClosestTarget(OutTargets, InDirection);

			if (IsValid(TargetActor))
			{
				if (IEITargetingInterface* NewTargeting = Cast<IEITargetingInterface>(TargetActor))
				{
					LockedTargetActor = TargetActor;
					NewTargeting->OnTargeted(true);
				}
			}
			else
			{
				StopLockOn();
			}
		}
	}
}

void UEITargetingComponent::FindTargets(TArray<AActor*>& OutTargetingActors) const
{
	TArray<FHitResult> OutHits;
	const FVector Start = Character->GetActorLocation();
	const FVector End = Start;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_OBJECT_TARGETING));

	TArray<AActor*> ActorsToIgnore;

	const bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Start,
		End,
		TargetingRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		DrawDebugType,
		OutHits,
		true);

	if (bHit)
	{
		for (const FHitResult& HitResult : OutHits)
		{
			AActor* HitActor = HitResult.GetActor();
			if (IEITargetingInterface* Targeting = Cast<IEITargetingInterface>(HitActor))
			{
				if (Targeting->CanBeTargeted())
				{
					OutTargetingActors.Add(HitActor);
				}
			}
		}
	}
}

// 후보군들 중 대상 하나를 골라 온다.
AActor* UEITargetingComponent::FindClosestTarget(TArray<AActor*>& InTargets, ESwitchingDirection InDirection /*= ESwitchingDirection::None*/) const
{
	float TargetCompareValue = 0.f;
	AActor* ClosestTarget = nullptr;

	for (const AActor* TargetActor : InTargets)
	{
		if (InDirection == ESwitchingDirection::None && LockedTargetActor == TargetActor)
		{
			continue;
		}

		FHitResult OutHit;
		const FVector Start = Camera->GetComponentLocation();
		const FVector End = TargetActor->GetActorLocation();

		TArray<AActor*> ActorsToIgnore;

		// 대상의 ECC_Visibility 채널에 대해서 Response 가 Block 으로 설정되어 있는지 확인 필요
		const bool bHit = UKismetSystemLibrary::LineTraceSingle(
			GetOwner(),
			Start,
			End,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore,
			DrawDebugType,
			OutHit,
			true);

		if (bHit)
		{
			// 카메라의 시야를 기준으로 하기 때문에 내적 계산은 카메라 기준으로 처리한다.
			// Left 방향에 따른 내적 계산
			if (InDirection == ESwitchingDirection::Left)
			{
				// 비교대상 Vector가 Normal 방향이기 때문에 반대로 계산한다.
				if (FVector::DotProduct(Camera->GetRightVector(), OutHit.Normal) > 0.f == false)
				{
					continue;
				}
			}

			// Right 방향에 따른 내적 계산
			if (InDirection == ESwitchingDirection::Right)
			{
				if (FVector::DotProduct(Camera->GetRightVector(), OutHit.Normal) < 0.f == false)
				{
					continue;
				}
			}

			AActor* HitActor = OutHit.GetActor();

			// 플레이어와 가장 가까운 타켓을 찾는다.
			// 전방에 있는 가장 가까운 타겟을 찾기 위해서 카메라의 ForwardVector와 LookAt Vector를 내적한다.
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), HitActor->GetActorLocation());
			float CheckValue = FVector::DotProduct(Camera->GetForwardVector(), LookAtRotation.Vector());

			if (CheckValue > TargetCompareValue)
			{
				TargetCompareValue = CheckValue;
				ClosestTarget = HitActor;
			}
		}
	}

	return ClosestTarget;
}

void UEITargetingComponent::OrientCamera() const
{
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void UEITargetingComponent::OrientMovement() const
{
	Character->GetCharacterMovement()->bOrientRotationToMovement = true;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void UEITargetingComponent::FaceLockOnActor() const
{
	const FRotator CurrentControlRotation = Character->GetControlRotation();

	const FVector TargetLocation = LockedTargetActor->GetActorLocation() - FVector(0.f, 0.f, 150.f);
	const FRotator TargetLookAtRotation = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), TargetLocation);

	FRotator InterpRotation = FMath::RInterpTo(CurrentControlRotation, TargetLookAtRotation, GetWorld()->GetDeltaSeconds(), FaceLockOnRotationSpeed);

	Character->GetController()->SetControlRotation(FRotator(InterpRotation.Pitch, InterpRotation.Yaw, CurrentControlRotation.Roll));
}

void UEITargetingComponent::LockOnTarget()
{
	TArray<AActor*> OutTargets;
	FindTargets(OutTargets);
	AActor* TargetActor = FindClosestTarget(OutTargets);

	if (IsValid(TargetActor))
	{
		LockedTargetActor = TargetActor;
		bIsLockOn = true;
		if (IEITargetingInterface* Targeting = Cast<IEITargetingInterface>(LockedTargetActor))
		{
			Targeting->OnTargeted(bIsLockOn);
		}

		OrientCamera();
	}
}

void UEITargetingComponent::StopLockOn()
{
	bIsLockOn = false;
	if (IEITargetingInterface* Targeting = Cast<IEITargetingInterface>(LockedTargetActor))
	{
		Targeting->OnTargeted(bIsLockOn);
	}
	LockedTargetActor = nullptr;
	OrientMovement();
}

