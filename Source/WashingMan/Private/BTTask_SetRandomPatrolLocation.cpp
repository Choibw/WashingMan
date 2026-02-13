// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SetRandomPatrolLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PatrolArea.h"
#include "DrawDebugHelpers.h"

UBTTask_SetRandomPatrolLocation::UBTTask_SetRandomPatrolLocation()
{
    NodeName = TEXT("Set Random Patrol Location");
}

EBTNodeResult::Type UBTTask_SetRandomPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    // PatrolArea 가져오기
    UObject* AreaObj = BB->GetValueAsObject(PatrolAreaKey.SelectedKeyName);
    APatrolArea* Area = Cast<APatrolArea>(AreaObj);
    if (!Area) return EBTNodeResult::Failed;

    UWorld* World = Area->GetWorld();
    if (!World) return EBTNodeResult::Failed;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
    if (!NavSys) return EBTNodeResult::Failed;
    
    // Area 기준 랜덤 목적지
    const FVector Origin = Area->GetActorLocation();
    const float Radius = Area->Radius;

    FNavLocation Out;
    if (!NavSys->GetRandomReachablePointInRadius(Origin, Radius, Out))
        return EBTNodeResult::Failed;

    BB->SetValueAsVector(PatrolLocationKey.SelectedKeyName, Out.Location);
    UE_LOG(LogTemp, Warning, TEXT("[AI] %s new patrol = %s (Area=%s)"),
        *GetNameSafe(AICon->GetPawn()),
        *Out.Location.ToString(),
        *GetNameSafe(Area));


    DrawDebugSphere(World, Out.Location, 25.f, 8, FColor::Yellow, false, 1.0f);

    return EBTNodeResult::Succeeded;
}

