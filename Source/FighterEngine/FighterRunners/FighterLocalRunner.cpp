// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterLocalRunner.h"

#include "Battle/Actors/FighterGameState.h"
#include "UnrealBattle/Actors/FighterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealBattle/Actors/FighterPlayerController.h"

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
		for (int i = 0; i < UGameplayStatics::GetNumPlayerControllers(this); i++)
		{
			if (AFighterPlayerController* PlayerController = Cast<AFighterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),i)))
			{
				PlayerController->UpdateInput();
			}
		}
		
		for (ABattleActor* Object : FighterGameState->Objects)
		{
			if (Object->GetParent()->IsActive)
				Object->GetBoxes();
		}
		for (APlayerCharacter* Player : FighterGameState->Players)
			Player->GetBoxes();

		FighterGameState->InternalGameState.Get()->TickGameState();
		FighterGameState->Update();
		ElapsedTime -= ONE_FRAME;
		accumulatorBreaker++;
	}
}
