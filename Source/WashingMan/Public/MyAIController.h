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

    // Æ©´×°ª
    UPROPERTY(EditDefaultsOnly, Category = "Wander")
    float WanderRadius = 3000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Wander")
    float AcceptanceRadius = 10.f;

    UPROPERTY(EditDefaultsOnly, Category = "Wander")
    float MinWaitSec = 0.7f;

    UPROPERTY(EditDefaultsOnly, Category = "Wander")
    float MaxWaitSec = 2.0f;

    FTimerHandle WanderTimerHandle;
};
