// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCAIController.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

static bool GPrintDistanceToPlayerOnScreen = false;
FAutoConsoleVariableRef CVar_PrintDistanceToPlayerOnScreen(
	TEXT("Game.PrintDistanceToPlayer"),
	GPrintDistanceToPlayerOnScreen,
	TEXT("."),
	ECVF_Default
);

void ANPCAIController::BeginPlay()
{
	Super::BeginPlay();

	if (this->AIBehavior != nullptr)
	{
		RunBehaviorTree(this->AIBehavior);
	}
}

void ANPCAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// MoveAwayFromPawn(GetPlayerPawn());
}

APawn* ANPCAIController::GetPlayerPawn()
{
	// APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	const int32 PlayerIndex = { 0 };
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), PlayerIndex);

	return PlayerPawn;
}

void ANPCAIController::MoveToPawn(APawn* ThePawn)
{
	if (IsValid(ThePawn))
	{
		SetFocus(ThePawn, EAIFocusPriority::Move);

		EPathFollowingRequestResult::Type Result = {};
		MoveToActor(ThePawn, AcceptanceRadius);
	}
}


void ANPCAIController::MoveAwayFromPawn(APawn* ThePawn)
{
	if (IsValid(ThePawn))
	{
		APawn* This = GetPawn();
		const FVector PawnLocation = ThePawn->GetActorLocation();
		const FVector ThisLocation = This->GetActorLocation();

		const float DistanceToPlayer = This->GetDistanceTo(ThePawn);
		if (GEngine && GPrintDistanceToPlayerOnScreen)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Distance To Player: %.2f"), DistanceToPlayer));
		}

		if (DistanceToPlayer <= DangerDistanceFromPlayer)
		{
			// get rotation between AI and player
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ThisLocation, PawnLocation);
			// get the right direction
			const FVector TheDirection = UKismetMathLibrary::GetForwardVector(LookAtRotation);
			// run backwards destination (adding ThisLocation so it doesn't move in a boring straight line)
			const FVector TheDestination = (TheDirection * (-MaxDistanceToMove)) + ThisLocation;

			SetFocus(ThePawn);

			EPathFollowingRequestResult::Type Result = {};
			Result = MoveToLocation(TheDestination, AcceptanceRadius);

			if (Result == EPathFollowingRequestResult::RequestSuccessful)
			{
				// TODO: Take Damage
				// This->Destroy();
			}
		}
		else
		{
			ClearFocus(EAIFocusPriority::Gameplay);
			StopMovement();
		}
	}
}
