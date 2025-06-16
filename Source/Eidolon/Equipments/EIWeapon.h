// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipments/EIEquipment.h"
#include "GameplayTagContainer.h"
#include "EIWeapon.generated.h"

struct FGameplayTag;
class UEIMontageActionData;
class UEICombatComponent;
class UEIWeaponCollisionComponent;

UCLASS()
class EIDOLON_API AEIWeapon : public AEIEquipment
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName UnequipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Animation")
	TObjectPtr<UEIMontageActionData> MontageActionData;

// Component Section
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEIWeaponCollisionComponent> WeaponCollision;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEICombatComponent> CombatComp;

// Data Section
protected:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> StaminaCostMap;

	/* 기본 데미지 */
	UPROPERTY(EditAnywhere)
	float BaseDamage = 15.f;

	/* 데미지 승수 */
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> DamageMultiplierMap;

public:
	AEIWeapon();

public:
	virtual void EquipItem() override;

	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;

	float GetStaminaCost(const FGameplayTag& InTag) const;
	float GetAttackDamage() const;

	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; }
	FORCEINLINE FName GetUnequipSocketName() const { return UnequipSocketName; }
	FORCEINLINE UEIWeaponCollisionComponent* GetCollision() const { return WeaponCollision; }

public:
	/* 무기의 Collision에 검출된 Actor에 Damage 전달 */
	void OnHitActor(const FHitResult& Hit);
};
