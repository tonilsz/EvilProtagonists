#include "NPCAIController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"

static bool GPrintDistanceToPlayerOnScreen = false;
FAutoConsoleVariableRef CVar_PrintDistanceToPlayerOnScreen(
	TEXT("Game.PrintDistanceToPlayer"),
	GPrintDistanceToPlayerOnScreen,
	TEXT("."),
	ECVF_Default
);

static bool GSphereTracingDebugDraw = false;
FAutoConsoleVariableRef CVar_SphereTracingDebugDraw(
	TEXT("Game.SphereTracingDebugDraw"),
	GSphereTracingDebugDraw,
	TEXT("."),
	ECVF_Default
);

static constexpr float AcceptanceRadius = 25.0f;

void ANPCAIController::BeginPlay()
{
	Super::BeginPlay();

	GetAllActorsOfType<ANPCAIController>(GetWorld(), AvailableAIActorsToConnect);

	if (this->AIBehavior != nullptr)
	{
		RunBehaviorTree(this->AIBehavior);
	}
}

void ANPCAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForNPCToDisconnect();
}

APawn* ANPCAIController::GetPlayerPawn()
{
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
		}
		else
		{
			ClearFocus(EAIFocusPriority::Gameplay);
			StopMovement();
		}
	}
}

bool ANPCAIController::DamagePawn(APawn* ThePawn, float Damage)
{
	if (IsValid(ThePawn))
	{
		return true;
	}

	return false;
}

bool ANPCAIController::CanPawnTakeDamage(APawn* ThePawn)
{
	if (IsValid(ThePawn))
	{

		FVector Start = ThePawn->GetActorLocation();
		FVector End = ThePawn->GetActorLocation();

		TArray<AActor*> ActorsToIgnore =
		{
			ThePawn,
		};

		bool bTraceComplex = false;

		TArray<FHitResult> HitArray = {};
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes =
		{
			UEngineTypes::ConvertToObjectType(ECC_Pawn)
		};

		EDrawDebugTrace::Type DebugDraw = GSphereTracingDebugDraw ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, TraceRadius, ObjectTypes,
			bTraceComplex, ActorsToIgnore, DebugDraw, HitArray, true, FLinearColor::Blue, FLinearColor::Gray, 60.0f);

		if (bHit)
		{
			ConnectNPCs(HitArray);
		}

		return bHit;
	}

	return false;
}

bool ANPCAIController::CanPawnExplode(APawn* ThePawn)
{
	if (IsValid(ThePawn))
	{

		FVector Start = ThePawn->GetActorLocation();
		FVector End = ThePawn->GetActorLocation();

		TArray<AActor*> ActorsToIgnore =
		{
			ThePawn,
			GetPlayerPawn()
		};

		bool bTraceComplex = false;

		TArray<FHitResult> HitArray = {};
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes =
		{
			UEngineTypes::ConvertToObjectType(ECC_Pawn)
		};

		EDrawDebugTrace::Type DebugDraw = GSphereTracingDebugDraw ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, TraceRadius, ObjectTypes,
			bTraceComplex, ActorsToIgnore, DebugDraw, HitArray, true, FLinearColor::Blue, FLinearColor::Gray, 60.0f);

		if (bHit)
		{
			ConnectNPCs(HitArray);
		}

		return bHit;
	}

	return false;
}

void ANPCAIController::ConnectNPCs(const TArray<FHitResult>& HitActorsArray)
{
	// Connect
	for (const FHitResult& Hit : HitActorsArray)
	{
		AActor* HitActor = Hit.GetActor();
		if (auto FoundActor = ConnectedActors.Find(HitActor))
		{
			if (GEngine)
			{
				 // GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Orange, FString::Printf(TEXT("Actor Already Added: %s"), *HitActor->GetName()));
			}

			continue;
		}
		else
		{
			if (GEngine)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Orange, FString::Printf(TEXT("Added Hit Actor: %s"), *HitActor->GetName()));
			}

			ConnectedActors.Add(HitActor);
		}
	}

	// Print
	for (AActor* TheActor : ConnectedActors)
	{
		if (IsValid(TheActor))
		{
			if (GEngine)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Connected Actor: %s"), *TheActor->GetName()));
			}
		}
	}

	if (GEngine)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Total Connected Actors: %d"), ConnectedActors.Num()));
	}
}

void ANPCAIController::CheckForNPCToDisconnect()
{
	// Disconnect
	for (AActor* ConnectedNPC : ConnectedActors)
	{
		// Compare ConnectedActors with HitActorsArray:
		FVector Start = ConnectedNPC->GetActorLocation();
		FVector End = ConnectedNPC->GetActorLocation();

		TArray<AActor*> ActorsToIgnore =
		{
			ConnectedNPC,
			GetPlayerPawn()
		};

		bool bTraceComplex = false;

		TArray<FHitResult> HitArray = {};
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes =
		{
			UEngineTypes::ConvertToObjectType(ECC_Pawn)
		};

		EDrawDebugTrace::Type DebugDraw = GSphereTracingDebugDraw ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, TraceRadius, ObjectTypes,
			bTraceComplex, ActorsToIgnore, DebugDraw, HitArray, true, FLinearColor::Blue, FLinearColor::Gray, 60.0f);

		for (const FHitResult& Hit : HitArray)
		{
			AActor* HitActor = Hit.GetActor();
			if (auto FoundActor = ConnectedActors.Find(HitActor))
			{
				// keep
				if (GEngine)
				{
					// GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Orange, FString::Printf(TEXT("Actor Already Added: %s"), *HitActor->GetName()));
				}

				continue;
			}
			else
			{
				// remove
				if (GEngine)
				{
					// GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Orange, FString::Printf(TEXT("Removed Hit Actor: %s"), *HitActor->GetName()));
				}

				ConnectedActors.Remove(HitActor);
			}
		}
	}

	// Print
	for (AActor* TheActor : ConnectedActors)
	{
		if (IsValid(TheActor))
		{
			if (GEngine)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Connected Actor: %s"), *TheActor->GetName()));
			}
		}
	}

	if (GEngine)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Total Connected Actors: %d"), ConnectedActors.Num()));
	}
}
