// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_EICollision.h"
#include "Components/EICombatComponent.h"
#include "Components/EIWeaponCollisionComponent.h"
#include "Equipments/EIWeapon.h"

UAnimNotifyState_EICollision::UAnimNotifyState_EICollision(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/) : Super(ObjectInitializer)
{

}

void UAnimNotifyState_EICollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (const UEICombatComponent* CombatComponent = OwnerActor->GetComponentByClass<UEICombatComponent>())
		{
			const AEIWeapon* Weapon = CombatComponent->GetMainWeapon();
			if (IsValid(Weapon))
			{
				Weapon->GetCollision()->TurnOnCollision();
			}
		}
	}
}

void UAnimNotifyState_EICollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (const AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (const UEICombatComponent* CombatComponent = OwnerActor->GetComponentByClass<UEICombatComponent>())
		{
			const AEIWeapon* Weapon = CombatComponent->GetMainWeapon();
			if (IsValid(Weapon))
			{
				Weapon->GetCollision()->TurnOffCollision();
			}
		}
	}
}
