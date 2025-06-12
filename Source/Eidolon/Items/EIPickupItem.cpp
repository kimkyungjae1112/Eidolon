// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/EIPickupItem.h"
#include "Equipments/EIEquipment.h"
#include "EIDefine.h"

AEIPickupItem::AEIPickupItem()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(MeshComp);

	MeshComp->SetCollisionObjectType(COLLISION_OBJECT_INTERACTION);
	MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AEIPickupItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEIPickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEIPickupItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (EquipmentClass)
	{
		if (AEIEquipment* CDO = EquipmentClass->GetDefaultObject<AEIEquipment>())
		{
			MeshComp->SetStaticMesh(CDO->MeshAsset);
			MeshComp->SetSimulatePhysics(true);
		}
	}
}

void AEIPickupItem::Interact(AActor* Interactor)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Interactor;

	AEIEquipment* SpawnItem = GetWorld()->SpawnActor<AEIEquipment>(EquipmentClass, GetActorTransform(), SpawnParams);
	if (SpawnItem)
	{
		SpawnItem->EquipItem();
		Destroy();
	}
}

