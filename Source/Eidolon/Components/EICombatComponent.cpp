// Fill out your copyright notice in the Description page of Project Settings.


#include "EICombatComponent.h"
#include "Equipments/EIWeapon.h"
#include "Character/EICharacter.h"
#include "Items/EIPickupItem.h"

UEICombatComponent::UEICombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UEICombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UEICombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UEICombatComponent::SetWeapon(AEIWeapon* NewWeapon)
{
	if (IsValid(MainWeapon))
	{
		if (AEICharacter* OwnerCharacter = Cast<AEICharacter>(GetOwner()))
		{
			AEIPickupItem* PickupItem = GetWorld()->SpawnActorDeferred<AEIPickupItem>(AEIPickupItem::StaticClass(), OwnerCharacter->GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			PickupItem->SetEquipmentClass(MainWeapon->GetClass());
			PickupItem->FinishSpawning(GetOwner()->GetActorTransform());

			MainWeapon->Destroy();
		}
	}

	MainWeapon = NewWeapon;
}

