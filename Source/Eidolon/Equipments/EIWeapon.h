// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipments/EIEquipment.h"
#include "GameplayTagContainer.h"
#include "EIWeapon.generated.h"

struct FGameplayTag;
class UEIMontageActionData;
class UEICombatComponent;

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

protected:
	UPROPERTY()
	TObjectPtr<UEICombatComponent> CombatComp;

// Data Section
protected:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> StaminaCostMap;

public:
	AEIWeapon();

public:
	virtual void EquipItem() override;

	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;

	float GetStaminaCost(const FGameplayTag& InTag) const;

	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; }
	FORCEINLINE FName GetUnequipSocketName() const { return UnequipSocketName; }
};
