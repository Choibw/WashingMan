// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCCharacter.generated.h"

class APatrolArea;

UCLASS()
class WASHINGMAN_API ANPCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPCCharacter();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI|Patrol")
	APatrolArea* PatrolArea = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
