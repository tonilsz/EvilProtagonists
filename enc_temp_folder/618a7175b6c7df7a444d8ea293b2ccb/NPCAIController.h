// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPCAIController.generated.h"

/**
 * 
 */
UCLASS()
class SHEEPCHAIN_API ANPCAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AcceptanceRadius = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DangerDistanceFromPlayer = 250.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistanceToMove = 500.0f;

	/**
	 *
	*/
	UFUNCTION(BlueprintCallable)
	APawn* GetPlayerPawn();

	/**
	 *
	*/
	UFUNCTION(BlueprintCallable)
	void MoveToPawn(APawn* ThePawn);

	/**
	 *
	*/
	UFUNCTION(BlueprintCallable)
	void MoveAwayFromPawn(APawn* ThePawn);
};
