// Fill out your copyright notice in the Description page of Project Settings.


#include "ATrashItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "MyCharacter.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h" 
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogTrash, Log, All);

// Sets default values
AATrashItem::AATrashItem()
{
    PrimaryActorTick.bCanEverTick = false;

    // MeshComp 먼저 생성하고 루트로 지정
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    SetRootComponent(MeshComp);
    MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
    MeshComp->bRenderInMainPass = true;
    MeshComp->SetHiddenInGame(false);

    // Proximity는 자식으로 붙임
    Proximity = CreateDefaultSubobject<USphereComponent>(TEXT("Proximity"));
    Proximity->SetupAttachment(MeshComp);
    Proximity->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Proximity->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    Proximity->SetCollisionObjectType(ECC_WorldDynamic);
    Proximity->SetGenerateOverlapEvents(true);
    Proximity->InitSphereRadius(150.f);
}

// Called when the game starts or when spawned
void AATrashItem::BeginPlay()
{
    Super::BeginPlay();

    Proximity->OnComponentBeginOverlap.AddDynamic(this, &AATrashItem::HandleBeginOverlap);
    Proximity->OnComponentEndOverlap.AddDynamic(this, &AATrashItem::HandleEndOverlap);

    // 시각 확인
    DrawDebugSphere(GetWorld(), Proximity->GetComponentLocation(),
        Proximity->GetScaledSphereRadius(), 16, FColor::Green, false, 5.f);
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
    if (Proximity)
    {
        Proximity->SetSphereRadius(ProximityRadius);
    }
}

void AATrashItem::HandleBeginOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 /*OtherBodyIndex*/,
    bool /*bFromSweep*/,
    const FHitResult& /*SweepResult*/) 
{
    UE_LOG(LogTrash, Warning, TEXT("[Trash] BeginOverlap with Actor=%s, Comp=%s"),
        *GetNameSafe(OtherActor), *GetNameSafe(OtherComp));
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, TEXT("[Trash] ANY overlap begin"));

    if (AMyCharacter* Char = Cast<AMyCharacter>(OtherActor))
    {
        Char->NotifyEnterTrash(this);
        UE_LOG(LogTrash, Log, TEXT("[Trash] -> Character NotifyEnterTrash"));
    }
}


void AATrashItem::HandleEndOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 /*OtherBodyIndex*/)
{
    UE_LOG(LogTrash, Warning, TEXT("[Trash] EndOverlap with Actor=%s, Comp=%s"),
        *GetNameSafe(OtherActor), *GetNameSafe(OtherComp));
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("[Trash] ANY overlap end"));

    if (AMyCharacter* Char = Cast<AMyCharacter>(OtherActor))
    {
        Char->NotifyExitTrash(this);
        UE_LOG(LogTrash, Log, TEXT("[Trash] -> Character NotifyExitTrash"));
    }
}

