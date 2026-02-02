// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "NavigationSystem.h"

void AMyAIController::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("AIController BeginPlay: %s"), *GetName());

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("Possessed Pawn: %s"), ControlledPawn ? *ControlledPawn->GetName() : TEXT("NULL"));
    }

    UNavigationSystemV1* NavSys =
        UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys)
    {
        UE_LOG(LogTemp, Warning, TEXT("No NavSys")); return;
    }

    FNavLocation Loc;
    if (!NavSys->GetRandomReachablePointInRadius(ControlledPawn->GetActorLocation(), 3000.f, Loc))
    {
        UE_LOG(LogTemp, Warning, TEXT("GetRandomReachablePointInRadius FAILED"));
        return;
    }

    auto ReqID = MoveToLocation(Loc.Location, 5.f);
    UE_LOG(LogTemp, Warning, TEXT("MoveToLocation called. ReqID=%d  Target=%s"),
        (int32)ReqID, *Loc.Location.ToString());
}