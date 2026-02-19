// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ANPCCharacter::ANPCCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 컨트롤러 회전 끄기
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 이동 방향으로 캐릭터가 회전하게
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->bOrientRotationToMovement = true;

		// 회전 속도(튜닝 포인트) - 값 클수록 더 빨리 고개 돌림
		MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);

		// (선택) 이동 가속/감속 느낌 튜닝도 여기서 가능
		// MoveComp->MaxWalkSpeed = 200.f;
	}

}

// Called when the game starts or when spawned
void ANPCCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


