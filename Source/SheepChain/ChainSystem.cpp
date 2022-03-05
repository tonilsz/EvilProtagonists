// Fill out your copyright notice in the Description page of Project Settings.


#include "ChainSystem.h"

// Sets default values
AChainSystem::AChainSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChainSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChainSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool shouldCheck = true; // TODO;
	if (shouldCheck)
	{
		// if (sheep)
	}
}
