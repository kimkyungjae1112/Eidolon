// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/EIInteractInterface.h"
#include "EIPickupItem.generated.h"

class AEIEquipment;

UCLASS()
class EIDOLON_API AEIPickupItem 
	: public AActor
	, public IEIInteractInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AEIEquipment> EquipmentClass;

public:	
	AEIPickupItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	/* IEIInterface Implement */
	virtual void Interact(AActor* Interactor) override;

public:
	FORCEINLINE void SetEquipmentClass(const TSubclassOf<AEIEquipment>& NewEquipmentClass) { EquipmentClass = NewEquipmentClass; }
};
