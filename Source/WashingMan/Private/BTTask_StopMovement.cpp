// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_StopMovement.h"
#include "AIController.h"

UBTTask_StopMovement::UBTTask_StopMovement()
{
    NodeName = TEXT("Stop Movement");
}

EBTNodeResult::Type UBTTask_StopMovement::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory
)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon)
        return EBTNodeResult::Failed;

    AICon->StopMovement();
    return EBTNodeResult::Succeeded;
}


