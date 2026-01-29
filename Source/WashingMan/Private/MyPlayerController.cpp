// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Contexts
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			if (CurrentContext)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 나중에 확장: 깔끔하게 제거 (메뉴/전환/리로드 시 중복 방지)
    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsys =
            LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            for (UInputMappingContext* IMC : DefaultMappingContexts)
            {
                if (IMC)
                {
                    Subsys->RemoveMappingContext(IMC);
                }
            }
        }
    }

    Super::EndPlay(EndPlayReason);
}

