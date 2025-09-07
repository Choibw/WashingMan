// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MyCharacter.generated.h"

class AATrashItem;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogMyCharacter, Log, All);

UCLASS()
class WASHINGMAN_API AMyCharacter : public ACharacter
{
	
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected:

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MouseLookAction;

	/** Keyboard Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DashAction;

	/** Keyboard Backflip Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BackflipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CleanAction;

	// Dash 관련 변수
	FTimerHandle DashTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	bool bIsDashing = false;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DefaultWalkSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashSpeed = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDuration = 999999.9f;

	UPROPERTY()
	float DefaultAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashAcceleration = 999999.f;

	// Backflip 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backflip")
	bool bIsBackflipping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backflip")
	float BackflipDistance = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backflip")
	float BackflipDuration = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Backflip")
	float BackflipCheckRadius = 200.f;   // 반구 반경

	UPROPERTY(EditAnywhere, Category = "Backflip")
	float BackflipHalfAngleDeg = 10.f;  // 좌우 반각(= 총 20도)

	UPROPERTY(EditAnywhere, Category = "Backflip|Trace")
	float BackflipSphereRadius = 30.f;        // 스피어 트레이스 반경 (두께)

	UPROPERTY(EditAnywhere, Category = "Backflip|Trace")
	float LowObstacleMaxHeight = 60.f;        // '낮은 박스' 상판 최대 높이(발 기준)

	UPROPERTY(EditAnywhere, Category = "Backflip|Timing")
	float BackflipDesiredNoObstacle = 1.0f;   // 장애물 없을 때 목표 시간

	UPROPERTY(EditAnywhere, Category = "Backflip|Timing")
	float BackflipDesiredWithObstacle = 0.4f; // 장애물 있을 때 목표 시간


	FTimerHandle BackflipTimerHandle;

	// Stun 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
	bool bIsStunned = false;

	// 스턴 총 지속시간(초) — 에디터에서 조절
	UPROPERTY(EditAnywhere, Category = "Stun|Timing", meta = (ClampMin = "0.05", UIMin = "0.05"))
	float StunDesiredDuration = 2.0f;

	FTimerHandle StunTimerHandle;

	// 애니메이션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* BackflipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* StunMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DashSlideMontage;

	// 현재 쓰레기 근접 여부
	UPROPERTY(BlueprintReadOnly, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	bool bNearTrash = false;

	// 가까이에 있는 쓰레기(지금은 하나만 관리)
	UPROPERTY() // GC 보호용
		TWeakObjectPtr<AATrashItem> NearbyTrash;

public:

	// 얼마나 들고 있는지 (UI 바인딩용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trash", meta = (AllowPrivateAccess = "true"))
	int32 CarriedCount = 0;

	// 한 번에 들 수 있는 최대치 (밸런싱용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trash", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxCarry = 3;

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for dashing input */
	void Dash();

	void StartDash();

	void StopDash();

	void Backflip();        

	void StartBackflip();   

	void EndBackflip();     

	void StartStun();

	void EndStun();

	void HandleClean();

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles dash inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoDash();

	UFUNCTION()
	void OnCharacterHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 쓰레기 근접 인식 상태 조회 (UMG에서 바인딩 용)
	UFUNCTION(BlueprintPure, Category = "Interact")
	bool IsNearTrash() const { return bNearTrash; }

	// 쓰레기 액터가 근접/이탈을 알릴 때 캐릭터가 받는 콜백
	void NotifyEnterTrash(class AATrashItem* Trash);
	void NotifyExitTrash(class AATrashItem* Trash);

	UFUNCTION(BlueprintPure, Category = "Trash")
	bool CanCarryMore() const { return CarriedCount < MaxCarry; }

	// 소지 개수 증가 (안전 체크 포함, true=증가 성공)
	UFUNCTION(BlueprintCallable, Category = "Trash")
	bool TryAddCarry(int32 Amount = 1)
	{
		if (Amount <= 0) return false;
		if (CarriedCount + Amount > MaxCarry) return false;
		CarriedCount += Amount;
		return true;
	}

	// 모두 비우고 몇 개 비웠는지 반환 (드랍존에서 사용)
	UFUNCTION(BlueprintCallable, Category = "Trash")
	int32 EmptyCarry()
	{
		const int32 Out = CarriedCount;
		CarriedCount = 0;
		return Out;
	}

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
