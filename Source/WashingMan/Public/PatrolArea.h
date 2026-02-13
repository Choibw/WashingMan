// PatrolArea.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolArea.generated.h"

UCLASS()
class WASHINGMAN_API APatrolArea : public AActor
{
    GENERATED_BODY()

public:
    APatrolArea();

    // 구역 반경(랜덤 목적지 생성에 사용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol")
    float Radius = 800.f;

    // 게임(PIE) 중에만 반경을 디버그로 표시할지
    UPROPERTY(EditAnywhere, Category = "Debug")
    bool bDrawDebugInGame = true;

    // 디버그 구 갱신 주기(초). 0.1~0.5 추천
    UPROPERTY(EditAnywhere, Category = "Debug", meta = (ClampMin = "0.02", ClampMax = "5.0"))
    float DebugDrawInterval = 0.2f;

protected:
    virtual void BeginPlay() override;

private:
    void DrawDebugPatrolArea();

    FTimerHandle DebugDrawTimerHandle;

#if WITH_EDITORONLY_DATA
    UPROPERTY(VisibleAnywhere, Category = "Patrol")
    class UBillboardComponent* Sprite;
#endif
};
