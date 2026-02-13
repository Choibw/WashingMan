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

    // 입력: PatrolArea(Object)
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    struct FBlackboardKeySelector PatrolAreaKey;

    // 출력: PatrolLocation(Vector)
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    struct FBlackboardKeySelector PatrolLocationKey;
};
