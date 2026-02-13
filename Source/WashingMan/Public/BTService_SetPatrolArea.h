#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetPatrolArea.generated.h"

UCLASS()
class WASHINGMAN_API UBTService_SetPatrolArea : public UBTService_BlackboardBase
{
    GENERATED_BODY()

public:
    UBTService_SetPatrolArea();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    // 한 번만 세팅하고 끝낼지 옵션
    UPROPERTY(EditAnywhere, Category = "Service")
    bool bSetOnlyIfEmpty = true;
};
