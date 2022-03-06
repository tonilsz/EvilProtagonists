// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Engine/TargetPoint.h"
#include "NPCAIController.generated.h"

template<typename T>
static void GetAllActorsOfType(UWorld* World, TArray<T*>& Out)
{
	for (TActorIterator<T> It(World); It; ++It)
	{
		Out.Add(*It);
	}
}

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
	
	/**
	 *
	*/
	UFUNCTION(BlueprintCallable)
	bool DamagePawn(APawn* ThePawn, float Damage);

	/**
	 *
	*/
	UFUNCTION(BlueprintCallable)
	bool CanPawnTakeDamage(APawn* ThePawn);

	/**
	 *
	*/
	UFUNCTION(BlueprintCallable)
	bool CanPawnExplode(APawn* ThePawn);

	/**
	 *
	*/
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetConnectedActors()
	{
		TArray<AActor*> ConnectedActorsArray = {};

		for (AActor* ConnectedActor : ConnectedActors)
		{
			ConnectedActorsArray.Add(ConnectedActor);
		}

		return ConnectedActorsArray;
	}

private:
	/**
	 *
	*/
	void ConnectNPCs(const TArray<FHitResult>& HitActorsArray);

	/**
	 *
	*/
	void CheckForNPCToDisconnect();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DangerDistanceFromPlayer = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistanceToMove = 500.0f;

private:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior = nullptr;

	UPROPERTY(VisibleAnywhere)
	TSet<AActor*> ConnectedActors;

	TArray<ANPCAIController*> AvailableAIActorsToConnect;
};
