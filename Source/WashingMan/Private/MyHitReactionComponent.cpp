// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHitReactionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UMyHitReactionComponent::UMyHitReactionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called every frame
void UMyHitReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMyHitReactionComponent::PlayStunFeedback()
{
	PlayCameraShake();
}

void UMyHitReactionComponent::PlayCameraShake()
{
	AActor* owner = GetOwner();

	if (ACharacter* MyCh = Cast<ACharacter>(owner))
	{
		if (APlayerController* PC = Cast<APlayerController>(MyCh->GetController()))
		{
			if (StunCameraShake)
			{
				PC->ClientStartCameraShake(StunCameraShake, 1.0f);
			}
		}
	}
}