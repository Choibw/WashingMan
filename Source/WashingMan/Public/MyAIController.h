#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

class UBehaviorTree;

UCLASS()
class WASHINGMAN_API AMyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMyAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset = nullptr;
};
