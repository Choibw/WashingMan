// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"


AMyGameMode::AMyGameMode()
{
	//DefaultPawnClass = AMyCharacter::StaticClass();
	//PlayerControllerClass = AMyPlayerController::StaticClass();
}

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 카운트다운 시작
    RemainingSeconds = TimeLimitSeconds;
    GetWorldTimerManager().SetTimer(
        CountdownHandle, this, &AMyGameMode::OnCountdownTick, 1.0f, true, 1.0f);

    // 한 프레임 지연 캡슐 보정: 유지 필요성 체크 필요 **********************
    GetWorldTimerManager().SetTimer(OneFrameDelayHandle, [this]()
        {
            if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
            {
                if (APawn* P = PC->GetPawn())
                {
                    if (UCapsuleComponent* Cap = P->FindComponentByClass<UCapsuleComponent>())
                    {
                        Cap->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                        Cap->SetCollisionResponseToAllChannels(ECR_Ignore);
                        Cap->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
                        Cap->SetGenerateOverlapEvents(true);

                        UE_LOG(LogTemp, Warning, TEXT("[Capsule-DELAY] GenOverlap=%d, RespToWorldDynamic=%d"),
                            Cap->GetGenerateOverlapEvents(),
                            (int32)Cap->GetCollisionResponseToChannel(ECC_WorldDynamic));
                    }
                }
            }
        }, 0.0f, false);
}

void AMyGameMode::OnCountdownTick()
{
    RemainingSeconds = FMath::Max(0, RemainingSeconds - 1);

    // 디버그로 지금 남은 시간 띄우기(원하면 지워도 됨)
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green,
            FString::Printf(TEXT("Remaining: %d"), RemainingSeconds));
    }

    if (RemainingSeconds <= 0)
    {
        // 타이머 정지 후 종료 처리
        GetWorldTimerManager().ClearTimer(CountdownHandle);
        EndGame();
    }
}
    
void AMyGameMode::NotifyTrashCleaned()
{
    CleanedCount++;

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan,
            FString::Printf(TEXT("Cleaned: %d / %d"), CleanedCount, CleanTargetCount));

    if (CleanedCount >= CleanTargetCount)
    {
        GetWorldTimerManager().ClearTimer(CountdownHandle);
        EndGame();
    }
}

void AMyGameMode::EndGame()
{
    // 여기서 원하는 “게임 종료 연출” 수행:
    // 1) 일시정지
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->SetPause(true);
        // UI 모드 전환, 커서 표시 등도 가능:
        // PC->bShowMouseCursor = true;
        // FInputModeUIOnly InputMode; PC->SetInputMode(InputMode);
    }

    // 2) 메시지 출력
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("GAME OVER"));
    }

    // 3) 매치 종료 상태로 전환(원하면)
    // EndMatch();

    // 4) 레벨 리스타트/로딩(선택)
    // UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
    // 또는 메인메뉴로 돌아가기 등…
}