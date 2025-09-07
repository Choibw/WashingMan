// Fill out your copyright notice in the Description page of Project Settings.


#include "DropZone.h"
#include "Components/BoxComponent.h"
#include "MyCharacter.h"
#include "MyGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADropZone::ADropZone()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    // Pawn만 감지
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);

}

void ADropZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
    if (AMyCharacter* Ch = Cast<AMyCharacter>(OtherActor))
    {
        const int32 Carry = Ch->CarriedCount;
        if (Carry <= 0) return;

        if (AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this)))
        {
            // 헤더를 안 고쳤으니까 CleanedCount++ 함수 그대로 활용
            for (int32 i = 0; i < Carry; ++i)
            {
                GM->NotifyTrashCleaned();
            }
        }

        Ch->CarriedCount = 0;

        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("드랍 완료! (손 비워짐)"));
    }
}


// Called every frame
void ADropZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

