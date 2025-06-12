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

