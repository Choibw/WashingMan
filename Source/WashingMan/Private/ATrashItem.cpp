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

    // ▶ 캐릭터를 '막지 않도록' 변경
    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);      // 물리 블록 X, 쿼리만
    MeshComp->SetCollisionObjectType(ECC_WorldDynamic);
    MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);          // 기본은 전부 무시
    MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 라인 트레이스 등은 필요시 블록
    MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);   // Pawn은 Overlap(=막지 않음)

    MeshComp->SetMobility(EComponentMobility::Static);
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

