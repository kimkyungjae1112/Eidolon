// Fill out your copyright notice in the Description page of Project Settings.


#include "EIWeapon.h"
#include "Components/EICombatComponent.h"
#include "Components/EIWeaponCollisionComponent.h"
#include "Data/EIMontageActionData.h"
#include "EIGameplayTags.h"
#include "Kismet/GameplayStatics.h"

AEIWeapon::AEIWeapon()
{
	WeaponCollision = CreateDefaultSubobject<UEIWeaponCollisionComponent>(TEXT("WeaponCollision"));
	WeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);

	StaminaCostMap.Add(EIGameplayTags::Character_Attack_Light, 5.f);
	StaminaCostMap.Add(EIGameplayTags::Character_Attack_Running, 7.f);
	StaminaCostMap.Add(EIGameplayTags::Character_Attack_Special, 10.f);
	StaminaCostMap.Add(EIGameplayTags::Character_Attack_Heavy, 8.5f);

	DamageMultiplierMap.Add(EIGameplayTags::Character_Attack_Heavy, 1.8f);
	DamageMultiplierMap.Add(EIGameplayTags::Character_Attack_Running, 1.8f);
	DamageMultiplierMap.Add(EIGameplayTags::Character_Attack_Special, 2.1f);
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

		// 무기의 충돌 트레이스 컴포넌트에 무기 메쉬 컴포넌트를 설정한다.
		WeaponCollision->SetWeaponMesh(Mesh);

		// 무기를 소유한 OwnerActor를 충돌에서 무시한다.
		WeaponCollision->AddIgnoreActor(GetOwner());
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

float AEIWeapon::GetAttackDamage() const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		const FGameplayTag LastAttackType = CombatComp->GetLastAttackType();

		if (DamageMultiplierMap.Contains(LastAttackType))
		{
			const float Multiplier = DamageMultiplierMap[LastAttackType];
			return BaseDamage * Multiplier;
		}
	}
	return BaseDamage;
}

void AEIWeapon::OnHitActor(const FHitResult& Hit)
{
	AActor* TargetActor = Hit.GetActor();

	// 데미지 방향
	FVector DamageDirection = GetOwner()->GetActorForwardVector();

	// 데미지
	float AttackDamage = GetAttackDamage();

	UGameplayStatics::ApplyPointDamage(
		TargetActor,
		AttackDamage,
		DamageDirection,
		Hit,
		GetOwner()->GetInstigatorController(),
		this,
		nullptr
	);
}
