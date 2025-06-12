// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EIDefine.h"
#include "Components/ActorComponent.h"
#include "EIAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegateOnAttributeChanged, EIAttributeType, float)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EIDOLON_API UEIAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/* 스텟 변경 Delegate */
	FDelegateOnAttributeChanged OnAttributeChanged;

protected:
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float BaseStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRegenRate = 0.2f; 

	/** 스테미나 재생 타이머 */
	FTimerHandle StaminaRegenTimerHandle;

public:	
	UEIAttributeComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE float GetBaseStamina() const { return BaseStamina; }	
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }

	FORCEINLINE float GetStaminaRatio() const { return BaseStamina / MaxStamina; }

	/* 스테미나가 충분한지 체크 */
	bool CheckHasEnoughStamina(float StaminaCost) const;

	/* 스테미나 차감 */
	void DecreaseStamina(float StaminaCost);

	/* 스테미나 재충전/중지 토글 */
	void ToggleStaminaRegeneration(bool bEnabled, float StartDelay = 2.f);

	/* 스텟 변경시 브로드캐스트 Wrapper 함수 */
	void BroadcastAttributeChanged(const EIAttributeType& InAttributeType) const;

private:
	void RegenerateStaminaHandler();
};
