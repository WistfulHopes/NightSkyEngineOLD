// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterLocalRunner.h"

#include "FighterEngine/Battle/Actors/FighterGameState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFighterLocalRunner::AFighterLocalRunner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates=false;
}

// Called when the game starts or when spawned
void AFighterLocalRunner::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundFighterGameStates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFighterGameState::StaticClass(), FoundFighterGameStates);
	if(FoundFighterGameStates.Num()>0){
		FighterGameState = Cast<AFighterGameState>(FoundFighterGameStates[0]);
	}
}

void AFighterLocalRunner::Update(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	int accumulatorBreaker = 0;
	while (ElapsedTime >= ONE_FRAME && accumulatorBreaker < AccumulatorBreakerMax)
	{
		//while elapsed time is greater than one frame...
		FighterGameState->TickGameState();
		ElapsedTime -= ONE_FRAME ;
		accumulatorBreaker++;
	}
}
