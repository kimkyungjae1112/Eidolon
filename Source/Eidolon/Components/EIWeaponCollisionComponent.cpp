// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EIWeaponCollisionComponent.h"

UEIWeaponCollisionComponent::UEIWeaponCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
}


void UEIWeaponCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UEIWeaponCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsCollisionEnabled)
	{
		CollisionTrace();
	}
}

void UEIWeaponCollisionComponent::TurnOnCollision()
{
	AlreadyHitActors.Empty();
	bIsCollisionEnabled = true;
}

void UEIWeaponCollisionComponent::TurnOffCollision()
{
	bIsCollisionEnabled = false;
}

void UEIWeaponCollisionComponent::SetWeaponMesh(UPrimitiveComponent* MeshComponent)
{
	WeaponMesh = MeshComponent;
}

void UEIWeaponCollisionComponent::AddIgnoreActor(AActor* Actor)
{
	IgnoredActors.Add(Actor);
}

void UEIWeaponCollisionComponent::RemoveIgnoredActor(AActor* Actor)
{
	IgnoredActors.Remove(Actor);
}

bool UEIWeaponCollisionComponent::CanHitActor(AActor* Actor) const
{
	return AlreadyHitActors.Contains(Actor) == false;
}

void UEIWeaponCollisionComponent::CollisionTrace()
{
	TArray<FHitResult> OutHits;

	const FVector Start = WeaponMesh->GetSocketLocation(TraceStartSocketName);
	const FVector End = WeaponMesh->GetSocketLocation(TraceEndSocketName);

	const bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Start,
		End,
		TraceRadius,
		TraceObjectTypes,
		false,
		IgnoredActors,
		DrawDebugType,
		OutHits,
		true
	);

	if (bHit)
	{
		for (const FHitResult& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();

			if (HitActor == nullptr)
			{
				continue;
			}

			if (CanHitActor(HitActor))
			{
				AlreadyHitActors.Add(HitActor);

				// Call OnHitActor Broadcast
				if (OnHitActor.IsBound())
				{
					OnHitActor.Broadcast(Hit);
				}
			}

		}
	}
}

