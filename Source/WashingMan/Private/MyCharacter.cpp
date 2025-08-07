// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogMyCharacter);

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.0f;
	CameraBoom->bUsePawnControlRotation = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultAcceleration = GetCharacterMovement()->MaxAcceleration;
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyCharacter::OnCharacterHit);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMyCharacter::Dash);
		EnhancedInputComponent->BindAction(BackflipAction, ETriggerEvent::Started, this, &AMyCharacter::Backflip);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
	}
	else
	{
		UE_LOG(LogMyCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

void AMyCharacter::OnCharacterHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsDashing && !bIsStunned) // 대시 중에만 처리
	{
		UE_LOG(LogMyCharacter, Warning, TEXT("Hit during dash - starting stun"));
		StopDash();     // 대시 강제 종료
		StartStun();    // 스턴 상태 진입
	}
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	if (bIsBackflipping || bIsStunned) return;

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	if (bIsBackflipping || bIsStunned) return;

	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMyCharacter::Dash()
{
	if (bIsBackflipping || bIsStunned) return;

	// route the input
	DoDash();
}

void AMyCharacter::Backflip()
{
	if (bIsDashing && !bIsBackflipping && !bIsStunned)
	{
		// 타이머 제거 → StopDash가 중복으로 실행되는 것 방지
		GetWorldTimerManager().ClearTimer(DashTimerHandle);

		StopDash();  // 먼저 대시 중지
		StartBackflip();
	}
}

void AMyCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMyCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMyCharacter::DoDash()
{
	if (GetController() != nullptr && !GetWorldTimerManager().IsTimerActive(DashTimerHandle))
	{
		StartDash();
		UE_LOG(LogMyCharacter, Log, TEXT("Dash Started!"));
	}
}

void AMyCharacter::StartDash()
{
	UE_LOG(LogMyCharacter, Warning, TEXT("Trying to dash: bIsDashing=%s, bIsStunned=%s"),
		bIsDashing ? TEXT("true") : TEXT("false"),
		bIsStunned ? TEXT("true") : TEXT("false"));

	if (bIsDashing || bIsStunned)
	{
		UE_LOG(LogMyCharacter, Warning, TEXT("Dash prevented - conditions not met"));
		return;
	}

	bIsDashing = true;

	// 대시 속도로 변경
	GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
	GetCharacterMovement()->MaxAcceleration = DashAcceleration;
	UE_LOG(LogMyCharacter, Log, TEXT("Current MaxWalkSpeed: %f"), GetCharacterMovement()->MaxWalkSpeed);

	// 강제 이동
	AddMovementInput(GetActorForwardVector(), 1.0f);

	// 타이머로 일정 시간 뒤 다시 원래 속도로
	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &AMyCharacter::StopDash, DashDuration, false);
}

void AMyCharacter::StopDash()
{
	if (!bIsDashing) return;

	bIsDashing = false;

	GetWorldTimerManager().ClearTimer(DashTimerHandle);

	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	GetCharacterMovement()->MaxAcceleration = DefaultAcceleration;

	UE_LOG(LogMyCharacter, Log, TEXT("Dash Ended - Speed = %f"), GetCharacterMovement()->MaxWalkSpeed);
}

void AMyCharacter::StartBackflip()
{
	if (bIsBackflipping)
		return;

	UE_LOG(LogMyCharacter, Log, TEXT("Backflip Started!"));

	bIsBackflipping = true;

	if (BackflipMontage)
	{
		PlayAnimMontage(BackflipMontage);
	}

	// 이동 막기
	GetCharacterMovement()->DisableMovement();

	// 애니메이션 없더라도 회전 정지 효과
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	// ===== 앞 방향 장애물 감지 =====
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 200.f;  // 앞 방향 200cm 감지

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("BackflipTrace")), true, this);

	bool bObstacleAhead = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,  // or ECC_GameTraceChannel1 if custom
		TraceParams
	);

	// 장애물 유무에 따라 백플립 시간 설정
	float FlipDuration = bObstacleAhead ? 0.4f : 1.0f;

	UE_LOG(LogMyCharacter, Log, TEXT("Backflip Duration: %f (ObstacleAhead: %s)"),
		FlipDuration,
		bObstacleAhead ? TEXT("True") : TEXT("False"));

	// 1초 뒤 끝내기
	GetWorldTimerManager().SetTimer(
		BackflipTimerHandle,
		this,
		&AMyCharacter::EndBackflip,
		FlipDuration,
		false
	);
}

void AMyCharacter::EndBackflip()
{
	UE_LOG(LogMyCharacter, Log, TEXT("Backflip Ended!"));
	bIsBackflipping = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AMyCharacter::StartStun()
{
	bIsStunned = true;

	// 움직임 멈추기
	GetCharacterMovement()->DisableMovement();
	UE_LOG(LogMyCharacter, Warning, TEXT("Stunned for 2 seconds"));

	// 타이머로 복원 예약
	GetWorldTimerManager().SetTimer(StunTimerHandle, this, &AMyCharacter::EndStun, 2.0f, false);
}

void AMyCharacter::EndStun()
{
	bIsStunned = false;

	// 걷기 상태로 복원
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	UE_LOG(LogMyCharacter, Warning, TEXT("Stun ended"));
}