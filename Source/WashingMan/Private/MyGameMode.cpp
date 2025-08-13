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

    // 다음 틱에 실행 → 블루프린트가 뭘 해도 최종값을 우리가 덮음
    FTimerHandle Th;
    GetWorldTimerManager().SetTimer(Th, [this]()
        {
            if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
            {
                if (APawn* P = PC->GetPawn())
                {
                    if (UCapsuleComponent* Cap = P->FindComponentByClass<UCapsuleComponent>())
                    {
                        // 안전빵: 최소한 쿼리는 켜두기
                        Cap->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

                        // 테스트용: 일단 전부 무시 → WorldDynamic만 Overlap로
                        Cap->SetCollisionResponseToAllChannels(ECR_Ignore);
                        Cap->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

                        // 오버랩 이벤트 생성 보장
                        Cap->SetGenerateOverlapEvents(true);

                        UE_LOG(LogTemp, Warning, TEXT("[Capsule-DELAY] Profile=%s, Enabled=%d, GenOverlap=%d, RespToWorldDynamic=%d"),
                            *Cap->GetCollisionProfileName().ToString(),
                            (int32)Cap->GetCollisionEnabled(),
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