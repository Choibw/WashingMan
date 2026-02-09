// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SetRandomPatrolLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetRandomPatrolLocation::UBTTask_SetRandomPatrolLocation()
{
    NodeName = TEXT("Set Random Patrol Location");
}

EBTNodeResult::Type UBTTask_SetRandomPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon) return EBTNodeResult::Failed;

    APawn* P = AICon->GetPawn();
    if (!P) return EBTNodeResult::Failed;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(P->GetWorld());
    if (!NavSys) return EBTNodeResult::Failed;

    const FVector Origin = P->GetActorLocation();
    FNavLocation Out;

    if (!NavSys->GetRandomReachablePointInRadius(Origin, Radius, Out))
        return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    BB->SetValueAsVector(PatrolLocationKey.SelectedKeyName, Out.Location);
    return EBTNodeResult::Succeeded;
}

