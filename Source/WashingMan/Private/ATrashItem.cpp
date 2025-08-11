// Fill out your copyright notice in the Description page of Project Settings.


#include "ATrashItem.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "MyCharacter.h"

// Sets default values
AATrashItem::AATrashItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // 화면에 보일 메쉬 컴포넌트 생성
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    SetRootComponent(MeshComp);

    // 배치용 기본 설정 (원하면 취향에 맞게 조정)
    MeshComp->SetMobility(EComponentMobility::Static);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComp->SetCollisionObjectType(ECC_WorldStatic);
    MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
}

// Called when the game starts or when spawned
void AATrashItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AATrashItem::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // 에디터에서 지정한 메쉬/머티리얼을 즉시 반영
    if (MeshAsset)
    {
        MeshComp->SetStaticMesh(MeshAsset);
    }
    if (OverrideMaterial)
    {
        MeshComp->SetMaterial(0, OverrideMaterial);
    }
}

// Called every frame
void AATrashItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

