// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

UENUM(BlueprintType)
enum class ENPCState : uint8
{
	Idle,
	Wandering
};

UCLASS()
class WASHINGMAN_API AMyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
    AMyAIController();

protected:
    virtual void BeginPlay() override;
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
    void StartWander();
    void StopWander();

    void ScheduleNextMove(float MinDelay, float MaxDelay);
    void MoveToRandomLocation();

private:
    ENPCState State = ENPCState::Idle;

    // 튜닝값
    UPROPERTY(EditDefaultsOnly, Category = "Wander")
    float WanderRadius = 1000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Wander")
    float AcceptanceRadius = 10.f;

    UPROPERTY(EditDefaultsOnly, Category = "Wander")
    float MinWaitSec = 0.7f;

    UPROPERTY(EditDefaultsOnly, Category = "Wander")
    float MaxWaitSec = 2.0f;

    FTimerHandle WanderTimerHandle;

    // 플레이어 근접 반응 튜닝값
    UPROPERTY(EditDefaultsOnly, Category = "Wander|Player")
    float StopDistance = 300.f;

    UPROPERTY(EditDefaultsOnly, Category = "Wander|Player")
    float ResumeDistance = 650.f; // StopDistance보다 크게(진동 방지)

    UPROPERTY(EditDefaultsOnly, Category = "Wander|Player")
    float PlayerCheckInterval = 0.2f;

    FTimerHandle PlayerCheckTimerHandle;

    void StartPlayerProximityCheck();
    void StopPlayerProximityCheck();
    void CheckPlayerProximity();
};
