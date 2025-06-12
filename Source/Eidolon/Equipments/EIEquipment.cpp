// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/EIEquipment.h"
#include "GameFramework/Character.h"

AEIEquipment::AEIEquipment()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Equip Mesh Component"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

void AEIEquipment::BeginPlay()
{
	Super::BeginPlay();

}

void AEIEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEIEquipment::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MeshAsset)
	{
		Mesh->SetStaticMesh(MeshAsset);
	}
}

void AEIEquipment::EquipItem()
{
}

void AEIEquipment::UnequipItem()
{
}

void AEIEquipment::AttachToOwner(FName SocketName)
{
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* CharacterMesh = OwnerCharacter->GetMesh())
		{
			AttachToComponent(CharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
		}
	}
}

