// Fill out your copyright notice in the Description page of Project Settings.


#include "EIWeapon.h"
#include "Components/EICombatComponent.h"
#include "Data/EIMontageActionData.h"
#include "EIGameplayTags.h"

AEIWeapon::AEIWeapon()
{
	StaminaCostMap.Add(EIGameplayTags::Character_Attack_Light, 5.f);
	StaminaCostMap.Add(EIGameplayTags::Character_Attack_Running, 7.f);
	StaminaCostMap.Add(EIGameplayTags::Character_Attack_Special, 10.f);
	StaminaCostMap.Add(EIGameplayTags::Character_Attack_Heavy, 8.5f);
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

float AEIWeapon::GetStaminaCost(const FGameplayTag& InTag) const
{
	if (StaminaCostMap.Contains(InTag))
	{
		return StaminaCostMap[InTag];
	}
	return 0.0f;
}
