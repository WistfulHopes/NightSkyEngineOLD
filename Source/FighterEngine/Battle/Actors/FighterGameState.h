// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioManager.h"
#include "FighterParticleManager.h"
#include "GameFramework/GameStateBase.h"
#include "PlayerCharacter.h"
#include "FighterGameState.generated.h"

#define MAX_ROLLBACK_FRAMES 10
#define FRAME_ADVANTAGE_LIMIT 5
#define FRAME_DELAY 0
#define INITIAL_FRAME 10
#define FRAME_RATE 60
#define ONE_FRAME (1.0f / FRAME_RATE)

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

#pragma pack (push, 1)
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

USTRUCT(BlueprintType)
struct FBattleState
{
	GENERATED_BODY()

	char BattleStateSync;
	int CurrentScreenPos = 0;
	UPROPERTY(BlueprintReadOnly)
	int RoundTimer;
	bool PauseTimer;
	int Meter[2] { 0 , 0 };
	int MaxMeter[2] { 10000 , 10000 };
	int P1RoundsWon;
	int P2RoundsWon;
	FAudioChannel CommonAudioChannels[CommonAudioChannelCount];
	FAudioChannel CharaAudioChannels[CharaAudioChannelCount];
	FAudioChannel CharaVoiceChannels[CharaVoiceChannelCount];
	FAudioChannel AnnouncerVoiceChannel;
	
	char BattleStateSyncEnd;

	UPROPERTY(BlueprintReadOnly)
	ERoundFormat RoundFormat = ERoundFormat::FirstToTwo;
};
#pragma pack(pop)

#define SIZEOF_BATTLESTATE offsetof(FBattleState, BattleStateSyncEnd) - offsetof(FBattleState, BattleStateSync)

#pragma pack (push, 1)
USTRUCT()
struct FRollbackData
{
	GENERATED_BODY()
	FRollbackData();
	
	int FrameNumber;
	int ActiveObjectCount;
	uint8 ObjBuffer[406][SIZEOF_BATTLEACTOR] = { { 0 } };
	bool ObjActive[400] = { false };
	uint8 CharBuffer[6][SIZEOF_PLAYERCHARACTER] = { { 0 } };
	uint8 BattleStateBuffer[SIZEOF_BATTLESTATE] = { 0 };
	uint32 Checksum;
};
#pragma pack(pop)

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API AFighterGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY()
	APlayerCharacter* Players[6];
	UPROPERTY()
	ABattleActor* Objects[400];
	
	int FrameNumber;

	void HandlePushCollision(); //for each active object, handle push collision
	void HandleHitCollision(); //for each active object, handle hit collision
	void SetScreenBounds(); //sets screen bounds
	void SetWallCollision(); //forces wall collision
	void CollisionView(); //for each active object, display collision
	
public:
	UPROPERTY()
	TArray<FRollbackData> RollbackData;
	FBattleState BattleState;
	
	UPROPERTY(BlueprintReadWrite)
	AAudioManager* AudioManager;
	
	ABattleActor* SortedObjects[406];

	UPROPERTY(BlueprintReadWrite)
	AFighterParticleManager* ParticleManager;
	UPROPERTY(EditAnywhere)
	int32 MaxOtgCount = 1;
	
	void Init(); //initializes game state
	void Update(int Input1, int Input2); //updates game state
	void SaveGameState(); //saves game state
	void LoadGameState(); //loads game state
	
	int LocalFrame;
	int RemoteFrame;
	int ActiveObjectCount;
private:
	int SyncFrame;
	int LocalFrameAdvantage;
	int RemoteFrameAdvantage;
	int LocalInputs[MAX_ROLLBACK_FRAMES][2];
	int RemoteInputs[MAX_ROLLBACK_FRAMES][2];
	float ElapsedTime;
	uint32 Checksum;
	uint32 OtherChecksum;
	int32 OtherChecksumFrame;
	int32 PrevOtherChecksumFrame;
	int32 ReconnectTime;
	bool SyncTestError;
	int SyncTestErrorCount;
	AFighterGameState();
	
	virtual void BeginPlay() override;

	void UpdateLocalInput(); //updates local input
	bool NeedRollback(); //checks if rollback is needed
	bool TimeSynced(); //checks if time is synchronized
	void HandleRoundWin();
	void HandleMatchWin();
	void RoundInit();

public:
	void UpdateRemoteInput(int RemoteInput[], int32 InFrame, int32 InFrameAdvantage); //when remote inputs are received, update inputs
	void SetOtherChecksum(uint32 RemoteChecksum, int32 InFrame);
	bool CheckForDesyncs();
	void SortObjects();

	virtual void Tick(float DeltaSeconds) override;
	void TickGameState(); //rollback operations, then updates game state
	
	int GetLocalInputs(int Index); //get local inputs from player controller
	
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
		
	void UpdateCamera();
	void UpdateUI();
	void PlayLevelSequence(APlayerCharacter* Target, ULevelSequence* Sequence);
	ABattleActor* AddBattleActor(UState* InState, int PosX, int PosY, bool FacingRight, APlayerCharacter* Parent); //creates object
	void StartSuperFreeze(int Duration);
	void BattleHudVisibility(bool Visible);
	void PlayCommonAudio(USoundBase* InSoundWave, float MaxDuration);
	void PlayCharaAudio(USoundBase* InSoundWave, float MaxDuration);
	void PlayVoiceLine(USoundBase* InSoundWave, float MaxDuration, int Player);
	void PlayAllAudio();

	UPROPERTY()
	class AFighterLocalRunner* FighterRunner;
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* SequenceTarget;
};
