// Fill out your copyright notice in the Description page of Project Settings.


#include "EIWeapon.h"
#include "Components/EICombatComponent.h"
#include "Data/EIMontageActionData.h"

AEIWeapon::AEIWeapon()
{
}

void AEIWeapon::EquipItem()
{
	Super::EquipItem();

	CombatComp = GetOwner()->GetComponentByClass<UEICombatComponent>();

	if (CombatComp)
	{
		CombatComp->SetWeapon(this);

		const FName AttachSocket = CombatComp->IsCombatEnabled() ? EquipSocketName : UnequipSocketName;

		AttachToOwner(AttachSocket);
	}
}

UAnimMontage* AEIWeapon::GetMontageForTag(const FGameplayTag& Tag, const int32 Index) const
{
	return MontageActionData->GetMontageForTag(Tag, Index);
}
