// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_EIAttackFinished.h"
#include "Character/EICharacter.h"
#include "Components/EIStateComponent.h"

UAnimNotify_EIAttackFinished::UAnimNotify_EIAttackFinished(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAnimNotify_EIAttackFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AEICharacter* Character = Cast<AEICharacter>(MeshComp->GetOwner()))
	{
		Character->AttackFinished(ComboResetDelay);
	}
}
