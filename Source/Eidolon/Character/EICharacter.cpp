// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EICharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/EIAttributeComponent.h"
#include "Components/EIStateComponent.h"
#include "Components/EICombatComponent.h"
#include "UI/EIPlayerHUDWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interface/EIInteractInterface.h"
#include "Data/EIMontageActionData.h"
#include "Equipments/EIWeapon.h"
#include "EIGameplayTags.h"
#include "EIDefine.h"
#include "Eidolon.h"


AEICharacter::AEICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	/* 이동, 감속 속도 */
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	AttributeComp = CreateDefaultSubobject<UEIAttributeComponent>(TEXT("Attribute Component"));
	StateComp = CreateDefaultSubobject<UEIStateComponent>(TEXT("State Component"));
	CombatComp = CreateDefaultSubobject<UEICombatComponent>(TEXT("Combat Component"));
}

void AEICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UEIPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}
}

void AEICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, FString::Printf(TEXT("Stamina : %f"), AttributeComp->GetBaseStamina()));
	GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::Cyan, FString::Printf(TEXT("MaxWalkSpeed : %f"), GetCharacterMovement()->MaxWalkSpeed));
}

void AEICharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AEICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Triggered, this, &ThisClass::Sprinting);
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Canceled, this, &ThisClass::Rolling);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Interaction);
		EnhancedInputComponent->BindAction(ToggleCombatAction, ETriggerEvent::Started, this, &ThisClass::ToggleCombat);

		// Combat 상태로 자동 전환
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::AutoToggleCombat);
		// 일반 공격
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &ThisClass::Attack);
		// 특수 공격
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ThisClass::SpecialAttack);
		// 강 공격
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ThisClass::HeavyAttack);
	}
}

bool AEICharacter::IsMoving() const
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		return MovementComp->Velocity.Size2D() > 3.f && MovementComp->GetCurrentAcceleration() != FVector::Zero();
	}
	return false;
}

bool AEICharacter::CanToggleCombat() const
{
	check(StateComp);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(EIGameplayTags::Character_State_Attacking);
	CheckTags.AddTag(EIGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(EIGameplayTags::Character_State_GeneralAction);

	return StateComp->IsCurrentStateEqualToAny(CheckTags) == false;
}

void AEICharacter::Move(const FInputActionValue& Value)
{
	check(StateComp);
	if (!StateComp->MovementInputEnabled()) return;

	FVector2D MovementInput = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementInput.Y);
		AddMovementInput(RightDirection, MovementInput.X);
	}
}

void AEICharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookInput.X);
		AddControllerPitchInput(LookInput.Y);
	}
}

void AEICharacter::Sprinting()
{
	if (AttributeComp->CheckHasEnoughStamina(5.f) && IsMoving())
	{
		AttributeComp->ToggleStaminaRegeneration(false);

		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;

		AttributeComp->DecreaseStamina(0.1f);
	}
	else
	{
		StopSprint();
	}
}

void AEICharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	AttributeComp->ToggleStaminaRegeneration(true);
}

void AEICharacter::Rolling()
{
	check(AttributeComp);
	check(StateComp);

	if (AttributeComp->CheckHasEnoughStamina(15.f))
	{
		// 스테미나 재충전 멈춤
		AttributeComp->ToggleStaminaRegeneration(false);

		// 이동입력 처리 무시
		StateComp->ToggleMovementInput(false);

		// 스테미나 차감
		AttributeComp->DecreaseStamina(15.f);

		// 구르기 애니메이션 재생
		PlayAnimMontage(RollingMontage);

		StateComp->SetState(EIGameplayTags::Character_State_Rolling);

		// 스테미나 재충전 시작
		AttributeComp->ToggleStaminaRegeneration(true, 1.5f);
	}

}

void AEICharacter::Interaction()
{
	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = Start;
	constexpr float Radius = 100.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_OBJECT_INTERACTION));

	TArray<AActor*> ActorsToIgnore;

	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true
	);

	if (bHit)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			if (IEIInteractInterface* Interaction = Cast<IEIInteractInterface>(HitActor))
			{
				Interaction->Interact(this);
			}
		}
	}
}

void AEICharacter::ToggleCombat()
{
	check(CombatComp);
	check(StateComp);

	if (const AEIWeapon* Weapon = CombatComp->GetMainWeapon())
	{
		if (CanToggleCombat())
		{
			StateComp->SetState(EIGameplayTags::Character_State_GeneralAction);

			if (CombatComp->IsCombatEnabled())
			{
				PlayAnimMontage(Weapon->GetMontageForTag(EIGameplayTags::Character_Action_Unequip));
			}
			else
			{
				PlayAnimMontage(Weapon->GetMontageForTag(EIGameplayTags::Character_Action_Equip));
			}
		}
	}
}

void AEICharacter::AutoToggleCombat()
{
	if (CombatComp)
	{
		if (!CombatComp->IsCombatEnabled())
		{
			ToggleCombat();
		}
	}
}

void AEICharacter::Attack()
{
	const FGameplayTag AttackTypeTag = GetAttackPerform();

	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}


void AEICharacter::SpecialAttack()
{
	const FGameplayTag AttackTypeTag = EIGameplayTags::Character_Attack_Special;

	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

void AEICharacter::HeavyAttack()
{
	const FGameplayTag AttackTypeTag = EIGameplayTags::Character_Attack_Heavy;

	if(CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

FGameplayTag AEICharacter::GetAttackPerform() const
{
	if (IsSprinting())
	{
		return EIGameplayTags::Character_Attack_Running;
	}
	return EIGameplayTags::Character_Attack_Light;
}

bool AEICharacter::CanPerformAttack(const FGameplayTag& AttackTypeTag) const
{
	check(StateComp);
	check(CombatComp);
	check(AttributeComp);

	if (IsValid(CombatComp->GetMainWeapon()) == false)
	{
		return false;
	}

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(EIGameplayTags::Character_State_Rolling);
	CheckTags.AddTag(EIGameplayTags::Character_State_GeneralAction);

	const float StaminaCost = CombatComp->GetMainWeapon()->GetStaminaCost(AttackTypeTag);

	return StateComp->IsCurrentStateEqualToAny(CheckTags) == false
		&& CombatComp->IsCombatEnabled()
		&& AttributeComp->CheckHasEnoughStamina(StaminaCost);
}

void AEICharacter::DoAttack(const FGameplayTag& AttackTypeTag)
{
	check(StateComp);
	check(AttributeComp);
	check(CombatComp);

	if (const AEIWeapon* Weapon = CombatComp->GetMainWeapon())
	{
		StateComp->SetState(EIGameplayTags::Character_State_Attacking);
		StateComp->ToggleMovementInput(false);
		CombatComp->SetLastAttackType(AttackTypeTag);

		AttributeComp->ToggleStaminaRegeneration(false);

		UAnimMontage* Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		if (!Montage)
		{
			// 콤보 한계 도달
			ComboCounter = 0;
			Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter);
		}

		PlayAnimMontage(Montage);

		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag);
		AttributeComp->DecreaseStamina(StaminaCost);
		AttributeComp->ToggleStaminaRegeneration(true, 1.5f);
	}
}

void AEICharacter::ExecuteComboAttack(const FGameplayTag& AttackTypeTag)
{
	if(StateComp->GetCurrentState() != EIGameplayTags::Character_State_Attacking)
	{
		if (bComboSequenceRunning && bCanComboInput == false)
		{
			// 애니메이션은 끝났지만 아직 콤보 시퀀스가 유효할 때 - 추가 입력 기회
			ComboCounter++;
			EI_LOG(LogEI, Display, TEXT("Additional Input : Combo Counter = %d"), ComboCounter);
		}
		else
		{
			EI_LOG(LogEI, Display, TEXT(">>> ComboSequence Started <<<"));
			ResetCombo();
			bComboSequenceRunning = true;
		}

		DoAttack(AttackTypeTag);
		GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	}
	else if (bCanComboInput)
	{
		bSavedComboInput = true;
	}
}

void AEICharacter::ResetCombo()
{
	EI_LOG(LogEI, Display, TEXT("Reset Combo"));

	bComboSequenceRunning = false;
	bCanComboInput = false;
	bSavedComboInput = false;
	ComboCounter = 0;
}

void AEICharacter::EnableComboWindow()
{
	bCanComboInput = true;
	EI_LOG(LogEI, Display, TEXT("Combo Window Opened: Combo Counter = %d"), ComboCounter);
}

void AEICharacter::DisableComboWindow()
{
	check(CombatComp);

	bCanComboInput = false;

	if (bSavedComboInput)
	{
		bSavedComboInput = false;
		ComboCounter++;
		EI_LOG(LogEI, Display, TEXT("Combo Window Closed: Advancing to next combo = %d"), ComboCounter);
		DoAttack(CombatComp->GetLastAttackType());
	}
	else
	{
		EI_LOG(LogEI, Display, TEXT("Combo Window Closed: No input received"));
	}
}

void AEICharacter::AttackFinished(const float ComboResetDelay)
{
	EI_LOG(LogEI, Display, TEXT("Attack Finished"));

	if (StateComp)
	{
		StateComp->ToggleMovementInput(true);
	}

	// ComboResetDelay 후에 콤보 시퀀스 종료
	GetWorld()->GetTimerManager().SetTimer(ComboResetTimerHandle, this, &ThisClass::ResetCombo, ComboResetDelay, false);
}

