// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_SetRandomPatrolLocation.generated.h"

UCLASS()
class WASHINGMAN_API UBTTask_SetRandomPatrolLocation : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_SetRandomPatrolLocation();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
    UPROPERTY(EditAnywhere, Category = "Patrol")
    float Radius = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector PatrolLocationKey;
};
