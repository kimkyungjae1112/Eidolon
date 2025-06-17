// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/EITargetingInterface.h"
#include "EIEnemy.generated.h"

class UEIAttributeComponent;
class UEIStateComponent;
class USoundCue;
class USphereComponent;
class UWidgetComponent;

UCLASS()
class EIDOLON_API AEIEnemy 
	: public ACharacter
	, public IEITargetingInterface
{
	GENERATED_BODY()

protected:
	/* 타겟팅 충돌 체크용 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> TargetingSphereComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEIAttributeComponent> AttributeComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEIStateComponent> StateComp;

	/* LockOn UI Widget*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> LockOnWidgetComp;

// Effect Section
protected:
	UPROPERTY(EditAnywhere, Category="Effect")
	TObjectPtr<USoundCue> ImpactSound;

	UPROPERTY(EditAnywhere, Category="Effect")
	TObjectPtr<UParticleSystem> ImpactParticle;

// Montage Section
protected:
	UPROPERTY(EditAnywhere, Category="Montage | HitReact")
	TObjectPtr<UAnimMontage> HitReactAnimFront;
	
	UPROPERTY(EditAnywhere, Category="Montage | HitReact")
	TObjectPtr<UAnimMontage> HitReactAnimBack;

	UPROPERTY(EditAnywhere, Category="Montage | HitReact")
	TObjectPtr<UAnimMontage> HitReactAnimLeft;

	UPROPERTY(EditAnywhere, Category="Montage | HitReact")
	TObjectPtr<UAnimMontage> HitReactAnimRight;

public:
	AEIEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	virtual void OnDeath();

protected:
	void ImpactEffect(const FVector& Location);
	void HitReaction(const AActor* Attacker);
	UAnimMontage* GetHitReactAnimation(const AActor* Attacker) const;

public:
	// EITargetingInterface Implement
	/* 타겟팅시 처리할 로직 처리 */
	virtual void OnTargeted(bool bTargeted) override;
	/* 타겟팅 가능한지 체크 */
	virtual bool CanBeTargeted() override;
};
