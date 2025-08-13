// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATrashItem.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UNiagaraSystem;
class USoundBase;
class AMyCharacter;

UCLASS()
class WASHINGMAN_API AATrashItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AATrashItem();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Prop")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Interact")
	USphereComponent* Proximity;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// 에디터에서 드롭다운으로 메쉬/머티리얼 바꿔 끼울 수 있게
	UPROPERTY(EditAnywhere, Category = "Prop")
	UStaticMesh* MeshAsset;

	UPROPERTY(EditAnywhere, Category = "Prop")
	UMaterialInterface* OverrideMaterial;

	// 감지 반경(에디터에서 조절)
	UPROPERTY(EditAnywhere, Category = "Interact", meta = (ClampMin = "30", UIMin = "30"))
	float ProximityRadius = 120.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void HandleEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

};
