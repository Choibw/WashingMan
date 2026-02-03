// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

AMyAIController::AMyAIController()
{
    bAttachToPawn = true;
}

void AMyAIController::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("[AI] BeginPlay: %s"), *GetName());

    StartWander();
}

void AMyAIController::StartWander()
{
    State = ENPCState::Wandering;
    ScheduleNextMove(0.1f, 0.2f); // 시작하자마자 거의 바로 한 번 움직이기
}

void AMyAIController::StopWander()
{
    State = ENPCState::Idle;
    GetWorld()->GetTimerManager().ClearTimer(WanderTimerHandle);
    StopMovement();
}

void AMyAIController::ScheduleNextMove(float MinDelay, float MaxDelay)
{
    if (State != ENPCState::Wandering) return;

    const float Delay = FMath::RandRange(MinDelay, MaxDelay);
    GetWorld()->GetTimerManager().SetTimer(
        WanderTimerHandle,
        this,
        &AMyAIController::MoveToRandomLocation,
        Delay,
        false
    );
}

void AMyAIController::MoveToRandomLocation()
{
    if (State != ENPCState::Wandering) return;

    APawn* P = GetPawn();
    if (!P)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AI] MoveToRandomLocation: Pawn NULL"));
        ScheduleNextMove(1.0f, 1.5f);
        return;
    }

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AI] NavSys NULL"));
        ScheduleNextMove(1.0f, 1.5f);
        return;
    }

    const FVector Origin = P->GetActorLocation();
    FNavLocation Out;

    if (!NavSys->GetRandomReachablePointInRadius(Origin, WanderRadius, Out))
    {
        UE_LOG(LogTemp, Warning, TEXT("[AI] RandomReachable FAILED (Origin=%s, R=%.0f)"),
            *Origin.ToString(), WanderRadius);

        // 섬 밖/네비 끊김 등으로 실패할 수 있으니 잠깐 쉬고 재시도
        ScheduleNextMove(0.8f, 1.2f);
        return;
    }

    const FAIRequestID ReqId = MoveToLocation(Out.Location, AcceptanceRadius);

    UE_LOG(LogTemp, Warning, TEXT("[AI] MoveTo Req=%d Origin=%s Target=%s"),
        (int32)ReqId, *Origin.ToString(), *Out.Location.ToString());
}

void AMyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    UE_LOG(LogTemp, Warning, TEXT("[AI] MoveCompleted Req=%d Code=%s"),
        (int32)RequestID, *UEnum::GetValueAsString(Result.Code));

    if (State != ENPCState::Wandering) return;

    // 성공/실패 상관없이 NPC스럽게 잠깐 쉬고 다음 목적지로
    if (Result.Code == EPathFollowingResult::Success)
    {
        ScheduleNextMove(MinWaitSec, MaxWaitSec);
    }
    else
    {
        // Blocked/Aborted/Invalid 등: 조금 짧게 쉬고 재시도
        ScheduleNextMove(0.3f, 0.7f);
    }
}