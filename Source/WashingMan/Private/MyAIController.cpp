#include "MyAIController.h"
#include "BehaviorTree/BehaviorTree.h"

AMyAIController::AMyAIController()
{
    bAttachToPawn = true;
}

void AMyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
        UE_LOG(LogTemp, Warning, TEXT("[AI] RunBehaviorTree OK"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[AI] BehaviorTreeAsset is NULL"));
    }
}
