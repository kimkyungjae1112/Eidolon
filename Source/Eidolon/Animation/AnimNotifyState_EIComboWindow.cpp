// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_EIComboWindow.h"
#include "Character/EICharacter.h"

UAnimNotifyState_EIComboWindow::UAnimNotifyState_EIComboWindow(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAnimNotifyState_EIComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AEICharacter* Character = Cast<AEICharacter>(MeshComp->GetOwner()))
	{
		Character->EnableComboWindow();
	}
}

void UAnimNotifyState_EIComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AEICharacter* Character = Cast<AEICharacter>(MeshComp->GetOwner()))
	{
		Character->DisableComboWindow();
	}

}
