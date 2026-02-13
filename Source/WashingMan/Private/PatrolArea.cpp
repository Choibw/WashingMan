// PatrolArea.cpp
#include "PatrolArea.h"

#include "TimerManager.h"
#include "DrawDebugHelpers.h"

#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif

APatrolArea::APatrolArea()
{
    PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
    // 에디터에서 액터 위치를 쉽게 집기 위한 아이콘(원하면 BP에서 텍스처 지정)
    Sprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
    RootComponent = Sprite;
#else
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
#endif
}

void APatrolArea::BeginPlay()
{
    Super::BeginPlay();

    // 게임 중(PIE 포함)에만 디버그 반경 표시
    if (bDrawDebugInGame)
    {
        // Interval보다 살짝 긴 lifetime으로 끊김 없이 유지
        GetWorldTimerManager().SetTimer(
            DebugDrawTimerHandle,
            this,
            &APatrolArea::DrawDebugPatrolArea,
            DebugDrawInterval,
            true
        );

        // 시작하자마자 1번 그려주기(첫 interval 기다리지 않게)
        DrawDebugPatrolArea();
    }
}

void APatrolArea::DrawDebugPatrolArea()
{
    UWorld* World = GetWorld();
    if (!World) return;

    const float LifeTime = FMath::Max(0.05f, DebugDrawInterval + 0.05f);

    DrawDebugSphere(
        World,
        GetActorLocation(),
        Radius,
        32,
        FColor::Green,
        false,      // persistent=false (우리가 주기적으로 다시 그림)
        LifeTime,
        0,
        2.0f
    );
}
