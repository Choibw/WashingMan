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

	// Dash 관련 변수
	FTimerHandle DashTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	bool bIsDashing = false;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DefaultWalkSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashSpeed = 20000.f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDuration = 0.7f;

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

	FTimerHandle BackflipTimerHandle;

	// Stun 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
	bool bIsStunned = false;

	FTimerHandle StunTimerHandle;

	// 애니메이션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* BackflipMontage;


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
