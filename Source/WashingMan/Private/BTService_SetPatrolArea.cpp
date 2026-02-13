#include "BTService_SetPatrolArea.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NPCCharacter.h"   // ANPCCharacter
#include "PatrolArea.h"     // APatrolArea

UBTService_SetPatrolArea::UBTService_SetPatrolArea()
{
    NodeName = TEXT("Set PatrolArea From NPC");
    Interval = 0.5f;          
    RandomDeviation = 0.0f;
}

void UBTService_SetPatrolArea::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    // bSetOnlyIfEmpty면 이미 값 있으면 건드리지 않음
    if (bSetOnlyIfEmpty)
    {
        UObject* Existing = BB->GetValueAsObject(GetSelectedBlackboardKey());
        if (Existing) return;
    }

    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon) return;

    ANPCCharacter* NPC = Cast<ANPCCharacter>(AICon->GetPawn());
    if (!NPC) return;

    if (!NPC->PatrolArea) return;

    BB->SetValueAsObject(GetSelectedBlackboardKey(), NPC->PatrolArea);

    // 디버그
    // UE_LOG(LogTemp, Warning, TEXT("[BTS] PatrolArea set to %s"), *NPC->PatrolArea->GetName());
}
