// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MyCharacter.generated.h"

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

<<<<<<< HEAD
	// Dash °ü·Ã º¯¼ö
=======
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CleanAction;

	// ì¹´ë©”ë¼ ê´€ë ¨ ë³€ìˆ˜

	UPROPERTY(EditAnywhere, Category = "Camera|FOV")
	float DashFOVOffset = 12.0f;   // ê¸°ë³¸ FOVì—ì„œ +ëª‡ ë„ í• ì§€

	UPROPERTY(EditAnywhere, Category = "Camera|FOV")
	float FOVInterpSpeed = 10.0f;  // ê°’ í´ìˆ˜ë¡ ë¹¨ë¦¬ ì „í™˜(ë¶€ë“œëŸ½ê²Œ 8~15 ì¶”ì²œ)

	float DefaultFOV = 90.0f;
	float TargetFOV = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|Shake")
	TSubclassOf<UCameraShakeBase> StunCameraShake;

	// Dash ê´€ë ¨ ë³€ìˆ˜

>>>>>>> 3b5c2f7 (ëŒ€ì‰¬, ìŠ¤í„´ ì—°ì¶œ ì¶”ê°€)
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

	// Backflip °ü·Ã º¯¼ö
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backflip")
	bool bIsBackflipping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backflip")
	float BackflipDistance = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backflip")
	float BackflipDuration = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Backflip")
	float BackflipCheckRadius = 200.f;   // ¹Ý±¸ ¹Ý°æ

	UPROPERTY(EditAnywhere, Category = "Backflip")
	float BackflipHalfAngleDeg = 10.f;  // ÁÂ¿ì ¹Ý°¢(= ÃÑ 20µµ)

	UPROPERTY(EditAnywhere, Category = "Backflip|Trace")
	float BackflipSphereRadius = 30.f;        // ½ºÇÇ¾î Æ®·¹ÀÌ½º ¹Ý°æ (µÎ²²)

	UPROPERTY(EditAnywhere, Category = "Backflip|Trace")
	float LowObstacleMaxHeight = 60.f;        // '³·Àº ¹Ú½º' »óÆÇ ÃÖ´ë ³ôÀÌ(¹ß ±âÁØ)

	FTimerHandle BackflipTimerHandle;

	// Stun °ü·Ã º¯¼ö
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
	bool bIsStunned = false;

	FTimerHandle StunTimerHandle;

	// ¾Ö´Ï¸ÞÀÌ¼Ç
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* BackflipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* StunMontage;

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

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
