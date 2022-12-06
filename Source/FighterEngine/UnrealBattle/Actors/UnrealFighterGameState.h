// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioManager.h"
#include "FighterParticleManager.h"
#include "GameFramework/GameStateBase.h"
#include "UnrealPlayerCharacter.h"
#include "UnrealFighterGameState.generated.h"

#define MAX_ROLLBACK_FRAMES 10
#define FRAME_ADVANTAGE_LIMIT 5
#define FRAME_DELAY 0
#define INITIAL_FRAME 10
#define FRAME_RATE 60
#define ONE_FRAME (1.0f / FRAME_RATE)

class FighterGameState;

UENUM()
enum class ERoundFormat : uint8
{
	FirstToOne,
	FirstToTwo,
	FirstToThree,
	FirstToFour,
	FirstToFive,
	TwoVsTwo,
	ThreeVsThree,
	TwoVsTwoKOF,
	ThreeVsThreeKOF,
};

USTRUCT()
struct FAudioChannel
{
	GENERATED_BODY()

	UPROPERTY()
	USoundBase* SoundWave;
	int StartingFrame;
	float MaxDuration = 1.0f;
	bool Finished = true;
};

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API AFighterGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	void CollisionView(); //for each active object, display collision

public:
	AFighterGameState();
	
	TSharedPtr<FighterGameState> InternalGameState;

	UPROPERTY()
	APlayerCharacter* Players[6];
	UPROPERTY()
	ABattleActor* Objects[400];;
	
	UPROPERTY(BlueprintReadWrite)
	AAudioManager* AudioManager;

	UPROPERTY(BlueprintReadWrite)
	AFighterParticleManager* ParticleManager;
	UPROPERTY(EditAnywhere)
	int32 MaxOtgCount = 1;
	
	void Init(); //initializes game state

private:
	virtual void BeginPlay() override;
	TSharedPtr<class UnrealInputDevice> DummyInputDevice;
public:
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere)
   	bool DisplayCollision;

	UPROPERTY(BlueprintReadWrite)
	class ALevelSequenceActor* SequenceActor;
	UPROPERTY(BlueprintReadWrite)
	ACameraActor* CameraActor;
	UPROPERTY(BlueprintReadWrite)
	ACameraActor* SequenceCameraActor;
	UPROPERTY(BlueprintReadWrite)
	class ABattleUIActor* BattleUIActor;

	void Update();
	
	void UpdateCamera();
	void UpdateUI();
	void PlayLevelSequence(APlayerCharacter* Target, ULevelSequence* Sequence);
	void BattleHudVisibility(bool Visible);
	void PlayCommonAudio(USoundBase* InSoundWave, float MaxDuration);
	void PlayCharaAudio(USoundBase* InSoundWave, float MaxDuration);
	void PlayVoiceLine(USoundBase* InSoundWave, float MaxDuration, int Player);
	void ManageAudio();
	void RollbackStartAudio();

	UPROPERTY()
	class AFighterLocalRunner* FighterRunner;
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* SequenceTarget;
};
