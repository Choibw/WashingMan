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
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "ATrashItem.h"
#include "MyGameMode.h"

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

	UCapsuleComponent* Cap = GetCapsuleComponent();
	if (Cap)
	{
		Cap->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		Cap->SetCollisionObjectType(ECC_Pawn);

		// 기본 프리셋을 Pawn으로
		Cap->SetCollisionProfileName(TEXT("Pawn"));

		// 트리거(드랍존 등)용: WorldDynamic은 Overlap
		Cap->SetGenerateOverlapEvents(true);
		Cap->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	}
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultAcceleration = GetCharacterMovement()->MaxAcceleration;
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyCharacter::OnCharacterHit);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);

	// WorldDynamic(=Trash Proximity) 과는 무조건 Overlap -> 추후 커스텀 채널로 리팩토링
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	auto* Cap = GetCapsuleComponent();
	UE_LOG(LogMyCharacter, Warning, TEXT("[CapsuleRuntime] Enabled=%d ObjType=%d ObstacleResp=%d WorldStaticResp=%d"),
		(int32)Cap->GetCollisionEnabled(),
		(int32)Cap->GetCollisionObjectType(),
		(int32)Cap->GetCollisionResponseToChannel(ECC_GameTraceChannel2),   // <- 너가 쓴 번호
		(int32)Cap->GetCollisionResponseToChannel(ECC_WorldStatic)
	);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDashing())
	{
		AddMovementInput(DashDirection, 1.0f);   // W를 누른 효과
	}
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
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMyCharacter::Dash);
		EnhancedInputComponent->BindAction(BackflipAction, ETriggerEvent::Started, this, &AMyCharacter::Backflip);

		// Looking
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

		// Cleaning
		EnhancedInputComponent->BindAction(CleanAction, ETriggerEvent::Started, this, &AMyCharacter::HandleClean);
	}
	else
	{
		UE_LOG(LogMyCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMyCharacter::OnCharacterHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsDashing() && !IsStunned())
	{
		UE_LOG(LogMyCharacter, Warning, TEXT("Hit during dash - starting stun"));
		StopDash();     // 대시 강제 종료
		StartStun();    // 스턴 상태 진입
	}
}

bool AMyCharacter::CanEnterState(EMyActionState NewState) const
{
	// 공통: 스턴 중에는 (원하면) Normal로만 복귀 허용 같은 정책 가능
	if (IsStunned() && NewState != EMyActionState::Normal)
		return false;

	switch (NewState)
	{
	case EMyActionState::Normal:   return true;
	case EMyActionState::Dashing:  return IsNormal();   // 노말에서만 대시
	case EMyActionState::Backflip: return IsDashing();  // 대시 중에만 백플립
	case EMyActionState::Stunned:  return true;         // 언제든 스턴 가능
	default: return false;
	}
}

bool AMyCharacter::CanMove() const { return !IsBackflip() && !IsStunned(); }
bool AMyCharacter::CanLook() const { return !IsBackflip() && !IsStunned() && !IsDashing(); }
bool AMyCharacter::CanDash() const { return IsNormal(); }
bool AMyCharacter::CanBackflip() const { return IsDashing(); }


void AMyCharacter::SetActionState(EMyActionState NewState)
{
	if (ActionState == NewState) return;
	if (!CanEnterState(NewState)) return;

	ExitState(ActionState);
	ActionState = NewState;
	EnterState(ActionState);

	UE_LOG(LogMyCharacter, Log, TEXT("[State] -> %d"), (int32)ActionState);
}

void AMyCharacter::EnterState(EMyActionState State)
{
	switch (State)
	{
	case EMyActionState::Normal:
		// (아직 비움) 다음 단계에서 Normal 복귀 시 처리(이동모드 복원 등)를 여기로 모을 예정
		break;

	case EMyActionState::Dashing:
	{
		// 대시 방향 구하기
		DashDirection = GetActorForwardVector();
		DashDirection.Z = 0.f;
		DashDirection.Normalize();
		UE_LOG(LogMyCharacter, Log, TEXT("DashDirection locked: %s"), *DashDirection.ToString());

		// 대시 속도로 변경
		GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
		GetCharacterMovement()->MaxAcceleration = DashAcceleration;
		UE_LOG(LogMyCharacter, Log, TEXT("Current MaxWalkSpeed: %f"), GetCharacterMovement()->MaxWalkSpeed);

		// 강제 이동(한 프레임 입력)
		AddMovementInput(GetActorForwardVector(), 1.0f);

		// 몽타주
		if (DashSlideMontage)
			PlayAnimMontage(DashSlideMontage, 1.0f);

		// 타이머로 대시 종료 예약 (StopDash는 이제 상태전환만 함)
		GetWorldTimerManager().SetTimer(
			DashTimerHandle,
			this,
			&AMyCharacter::StopDash,
			DashDuration,
			false
		);

		UE_LOG(LogMyCharacter, Log, TEXT("Dash Started - Speed = %f"), GetCharacterMovement()->MaxWalkSpeed);
		break;
	}

	case EMyActionState::Backflip:
	{
		UE_LOG(LogMyCharacter, Log, TEXT("Backflip Started!"));

		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->Velocity = FVector::ZeroVector;

		// ===== 전방 부채꼴 스피어 트레이스로 '벽/낮은 상자' 감지 =====
		const FVector Origin = GetActorLocation();

		FVector Forward = GetActorForwardVector();
		Forward.Z = 0.f;
		Forward.Normalize();

		// 캐릭터 발 위치(Z) 추정: 캡슐 하프하이트를 알고 있으면 더 정확
		float CapsuleHalfHeight = 0.f;
		if (const UCapsuleComponent* Cap = GetCapsuleComponent())
		{
			CapsuleHalfHeight = Cap->GetScaledCapsuleHalfHeight();
		}
		const float FeetZ = Origin.Z - CapsuleHalfHeight;

		// 감지할 오브젝트 타입
		TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjTypes;
		TraceObjTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2)); // Obstacle

		TArray<AActor*> Ignore;
		Ignore.Add(this);

		const int32 NumRays = 5; // -half ~ +half
		const float HalfRad = FMath::DegreesToRadians(BackflipHalfAngleDeg);

		// 여러 높이에서 검사 (낮은 박스 보정)
		const TArray<float> TraceHeights = { 30.f, 60.f, 100.f };

		bool bObstacleAhead = false;

		for (float Height : TraceHeights)
		{
			const FVector StartBase = FVector(
				Origin.X,
				Origin.Y,
				FeetZ + Height
			);

			for (int32 i = 0; i < NumRays; ++i)
			{
				const float T = (NumRays == 1) ? 0.f : (i / float(NumRays - 1)); // 0..1
				const float Angle = FMath::Lerp(-HalfRad, HalfRad, T);

				const FVector Dir = UKismetMathLibrary::RotateAngleAxis(
					Forward,
					FMath::RadiansToDegrees(Angle),
					FVector::UpVector
				);

				const FVector Start = StartBase;
				const FVector End = Start + Dir * BackflipCheckRadius;

				FHitResult Hit;
				const bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
					this,
					Start, End,
					BackflipSphereRadius,      // <-- 두께 줘서 빈틈 감소
					TraceObjTypes,
					/*bTraceComplex*/ false,
					Ignore,
					EDrawDebugTrace::None,
					Hit,
					/*bIgnoreSelf*/ true
				);

				if (!bHit) continue;

				// 면 법선
				const float AbsNZ = FMath::Abs(Hit.ImpactNormal.Z); // 0=수직, 1=수평

				// 1) 벽: 거의 수직면
				const bool bIsWall = (AbsNZ < 0.4f);

				// 2) 낮은 상자 윗면: 거의 수평면 + 발 기준 낮은 높이
				const bool bIsLowTop =
					(AbsNZ > 0.8f) &&
					((Hit.ImpactPoint.Z - FeetZ) <= LowObstacleMaxHeight);

				if (bIsWall || bIsLowTop)
				{
					bObstacleAhead = true;

					// 필요 시 디버그:
					// DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 12, FColor::Red, false, 0.5f);
					// DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor::Red, false, 0.5f, 0, 2.f);
					break;
				}
				// else // 필요 시 디버그:
				// DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.5f, 0, 1.f);
			}

			if (bObstacleAhead)
				break;
		}

		// ===== 분기별 목표 재생시간 -> 몽타주 배속 계산/재생 + 타이머 동기화 =====
		const float DesiredDuration = bObstacleAhead ? BackflipDesiredWithObstacle
			: BackflipDesiredNoObstacle;
		const float FinalDesired = FMath::Max(0.05f, DesiredDuration); // 0 방지

		float PlayRate = 1.f;
		if (BackflipMontage)
		{
			const float RawLen = BackflipMontage->GetPlayLength(); // 배속 1.0 기준 길이
			if (RawLen > KINDA_SMALL_NUMBER)
			{
				PlayRate = RawLen / FinalDesired;                   // 핵심 공식
				// 너무 과한 배속 방지
				PlayRate = FMath::Clamp(PlayRate, 0.1f, 5.0f);
			}

			PlayAnimMontage(BackflipMontage, PlayRate);
		}

		// 입력/잠금 해제 타이밍도 '원하는 시간'과 동일하게
		GetWorldTimerManager().ClearTimer(BackflipTimerHandle);
		GetWorldTimerManager().SetTimer(
			BackflipTimerHandle,
			this,
			&AMyCharacter::EndBackflip,
			FinalDesired,
			false
		);

		UE_LOG(LogMyCharacter, Log, TEXT("Backflip Desired=%.3f, PlayRate=%.3f, Obstacle=%s"),
			FinalDesired, PlayRate, bObstacleAhead ? TEXT("TRUE") : TEXT("FALSE"));

		break;
	}

	case EMyActionState::Stunned:
	{
		// 움직임 멈추기
		GetCharacterMovement()->DisableMovement();
		UE_LOG(LogMyCharacter, Warning, TEXT("Stunned for %.2f seconds"), StunDesiredDuration);

		const float Desired = FMath::Max(0.05f, StunDesiredDuration);
		float PlayRate = 1.f;

		// 애니메이션 재생
		if (StunMontage)
		{
			const float RawLen = StunMontage->GetPlayLength(); // 1.0배속 기준 길이
			if (RawLen > KINDA_SMALL_NUMBER)
			{
				PlayRate = RawLen / Desired;                   // 핵심 공식
				PlayRate = FMath::Clamp(PlayRate, 0.1f, 5.0f); // 과도한 배속 방지
			}

			// 원하는 시간에 딱 맞게 재생
			PlayAnimMontage(StunMontage, PlayRate);
		}

		// 타이머로 복원 예약
		GetWorldTimerManager().ClearTimer(StunTimerHandle);
		GetWorldTimerManager().SetTimer(StunTimerHandle, this, &AMyCharacter::EndStun, Desired, false);

		UE_LOG(LogMyCharacter, Log, TEXT("Stun Desired=%.3f, PlayRate=%.3f"), Desired, PlayRate);

		break;
	}

	default:
		break;
	}
}

void AMyCharacter::ExitState(EMyActionState State)
{
	switch (State)
	{
	case EMyActionState::Normal:
		break;

	case EMyActionState::Dashing:
	{
		// 타이머 정리 (이미 만료됐어도 safe)
		GetWorldTimerManager().ClearTimer(DashTimerHandle);

		// 속도/가속 원복
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		GetCharacterMovement()->MaxAcceleration = DefaultAcceleration;

		// 몽타주 정리
		if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
		{
			Anim->Montage_Stop(0.2f, DashSlideMontage);
		}

		UE_LOG(LogMyCharacter, Log, TEXT("Dash Ended - Speed = %f"), GetCharacterMovement()->MaxWalkSpeed);
		break;
	}

	case EMyActionState::Backflip:
	{
		UE_LOG(LogMyCharacter, Log, TEXT("Backflip Ended!"));

		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		break;
	}

	case EMyActionState::Stunned:
	{
		// 걷기 상태로 복원
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		// 안전: 대쉬 관련 깔끔하게 리셋
		GetWorldTimerManager().ClearTimer(DashTimerHandle);

		UE_LOG(LogMyCharacter, Warning, TEXT("Stun ended"));

		break;
	}

	default:
		break;
	}
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	if (!CanMove()) return;

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	if (!CanLook()) return;

	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMyCharacter::Dash()
{
	if (!CanDash()) return;
	
	DoDash();
}

void AMyCharacter::Backflip()
{
	if (CanBackflip())
	{
		// 타이머 제거 -> StopDash가 중복으로 실행되는 것 방지
		GetWorldTimerManager().ClearTimer(DashTimerHandle);

		StartBackflip();
	}
}

void AMyCharacter::DoMove(float Right, float Forward)
{
	if (IsDashing())
	{
		// 키보드 입력 막기
		Right = 0.f;                          
		Forward = 0.f;                      
	}

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
		if (Forward != 0.f) 
			AddMovementInput(ForwardDirection, Forward);
		if (Right != 0.f)
			AddMovementInput(RightDirection, Right);
	}
}

void AMyCharacter::DoLook(float Yaw, float Pitch)
{
	if (!GetController()) return;

	// 컨트롤러 회전
	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
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
	if (!CanDash())
	{
		UE_LOG(LogMyCharacter, Warning, TEXT("Dash prevented - conditions not met"));
		return;
	}

	SetActionState(EMyActionState::Dashing);
}

void AMyCharacter::StopDash()
{
	if (!IsDashing()) return;

	SetActionState(EMyActionState::Normal);
}

void AMyCharacter::StartBackflip()
{
	if (!CanBackflip()) return;

	SetActionState(EMyActionState::Backflip);
}

void AMyCharacter::EndBackflip()
{
	if (!IsBackflip()) return;

	SetActionState(EMyActionState::Normal);
}

void AMyCharacter::StartStun()
{
	if (IsStunned()) return;

	SetActionState(EMyActionState::Stunned);
}

void AMyCharacter::EndStun()
{
	if (!IsStunned()) return;

	SetActionState(EMyActionState::Normal);
}

void AMyCharacter::HandleClean()
{
	if (!bNearTrash || !NearbyTrash.IsValid())
	{
		UE_LOG(LogMyCharacter, Log, TEXT("[Clean] Pressed but no active trash"));
		return;
	}

	// 가방(손) 용량 체크
	if (CarriedCount >= MaxCarry)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("가방이 가득 찼습니다 (3/3)"));
		return;
	}

	AATrashItem* Target = NearbyTrash.Get();

	// 상태 먼저 정리(쓰레기를 파괴하면 이후 EndOverlap이 안 올 수도 있으니)
	NearbyTrash = nullptr;
	bNearTrash = false;

	// 소지 개수만 증가
	++CarriedCount;

	// 실제 제거 (멀티플레이면 서버 권한에서만)
	if (HasAuthority())
	{
		Target->Destroy();
		UE_LOG(LogMyCharacter, Log, TEXT("[Clean] Destroyed %s"), *GetNameSafe(Target));
	}
	else
	{
		// TODO: 멀티 도입 시 서버 RPC로 요청
		UE_LOG(LogMyCharacter, Warning, TEXT("[Clean] No authority; implement Server RPC for multiplayer"));
	}

	UE_LOG(LogMyCharacter, Log, TEXT("Clean (Carry %d/%d)"), CarriedCount, MaxCarry);
}

void AMyCharacter::NotifyEnterTrash(AATrashItem* Trash)
{
	NearbyTrash = Trash;            // 누가 가까이 있는지 보관
	if (!bNearTrash)                // 상태 변화 때만 로그 찍기
	{
		bNearTrash = true;
		UE_LOG(LogMyCharacter, Log, TEXT("IsNearTrash = TRUE (by %s)"), *GetNameSafe(Trash));
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("IsNearTrash = TRUE"));
	}
}

void AMyCharacter::NotifyExitTrash(AATrashItem* Trash)
{
	if (NearbyTrash.Get() == Trash) // 나와 관련된 쓰레기가 벗어났을 때만 해제
	{
		NearbyTrash = nullptr;
		if (bNearTrash)
		{
			bNearTrash = false;
			UE_LOG(LogMyCharacter, Log, TEXT("IsNearTrash = FALSE"));
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("IsNearTrash = FALSE"));
		}
	}
}