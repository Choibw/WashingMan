// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WASHINGMAN_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** Constructor */
	AMyGameMode();

    /** 현재 남은 시간(초). ui에서 바인딩할 수 있게 공개 */
    UPROPERTY(BlueprintReadOnly, Category = "Rules")
    int32 RemainingSeconds;

    /** 지금까지 청소한 개수 – ui 바인딩용 */
    UPROPERTY(BlueprintReadOnly, Category = "Rules")
    int32 CleanedCount = 0;

    /** 캐릭터가 쓰레기를 청소했을 때 호출 */
    UFUNCTION(BlueprintCallable, Category = "Rules")
    void NotifyTrashCleaned();

    /** 수동으로 즉시 게임 종료하고 싶을 때 호출(디버그/테스트용) */
    UFUNCTION(BlueprintCallable, Category = "Rules")
    void EndGame();

protected:
    virtual void BeginPlay() override;

    /** 1초마다 호출되는 틱 함수 */
    UFUNCTION()
    void OnCountdownTick();

    /** 총 제한 시간(초) */
    UPROPERTY(EditDefaultsOnly, Category = "Rules", meta = (ClampMin = "1", UIMin = "1"))
    int32 TimeLimitSeconds = 30;

    /** 목표 청소 개수 */
    UPROPERTY(EditAnywhere, Category = "Rules", meta = (ClampMin = "1", UIMin = "1"))
    int32 CleanTargetCount = 3;

private:
    FTimerHandle CountdownHandle;
    FTimerHandle OneFrameDelayHandle; // 기존 지연 타이머 유지
};
