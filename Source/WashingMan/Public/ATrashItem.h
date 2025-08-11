// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATrashItem.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UNiagaraSystem;
class USoundBase;

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// 에디터에서 드롭다운으로 메쉬/머티리얼 바꿔 끼울 수 있게
	UPROPERTY(EditAnywhere, Category = "Prop")
	UStaticMesh* MeshAsset;

	UPROPERTY(EditAnywhere, Category = "Prop")
	UMaterialInterface* OverrideMaterial;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
