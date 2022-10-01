// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleActor.h"
#include "FighterEngine/Battle/InputBuffer.h"
#include "FighterEngine/DataAssets/SequenceData.h"
#include "FighterEngine/DataAssets/SoundData.h"
#include "FighterEngine/DataAssets/StateDataAsset.h"
#include "FighterEngine/DataAssets/ParticleData.h"
#include "FighterEngine/Battle/StateMachine.h"
#include "FighterEngine/Battle/Subroutine.h"
#include "FighterEngine/DataAssets/SubroutineData.h"
#include "PlayerCharacter.generated.h"
#pragma pack (push, 1)

constexpr int CancelArraySize = 50;

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API APlayerCharacter : public ABattleActor
{
	GENERATED_BODY()
public:
	APlayerCharacter();
	
	unsigned char PlayerSync; //starting from this until PlayerSyncEnd, everything is saved/loaded for rollback
	int32 EnableFlags;
	int32 CurrentHealth;
protected:
	//internal variables	
	int32 CurrentAirJumpCount;
	int32 CurrentAirDashCount;
	int32 AirDashTimerMax;
	bool JumpCancel;
	bool FAirDashCancel;
	bool BAirDashCancel;
	bool SpecialCancel;
	bool SuperCancel;
	bool DefaultLandingAction;
	bool FarNormalForceEnable;
	int32 ThrowRange;
	
public:
	FWallBounceEffect WallBounceEffect;
	FGroundBounceEffect GroundBounceEffect;
	bool IsDead;
	bool ThrowActive;
	bool IsStunned;
	bool IsThrowLock;
	bool IsOnScreen;
	bool DeathCamOverride;
	int32 TeamIndex;
	int32 Inputs;
	int32 ActionFlags;
	int32 AirDashTimer;
	int32 AirDashNoAttackTime;
	int32 PlayerIndex;
	int32 Hitstun = -1;
	int32 Blockstun = -1;
	int32 Untech = -1;
	int32 KnockdownTime = -1;
	int32 TotalProration = 10000;
	int32 ComboCounter = 0;
	int32 LoopCounter = 0;
	int32 HasBeenOTG;
	bool TouchingWall;
	bool ChainCancelEnabled = true;
	bool WhiffCancelEnabled;
	bool StrikeInvulnerable;
	bool ThrowInvulnerable;
	bool HeadInvulnerable;
	int RoundWinTimer = 300;
	bool RoundWinInputLock;
	
	//movement values
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FDashInitSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FDashAccel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FDashMaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FDashFriction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BDashSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BDashHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BDashGravity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 JumpHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FJumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BJumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 JumpGravity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SuperJumpHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FSuperJumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BSuperJumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SuperJumpGravity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AirDashMinimumHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FAirDashSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BAirDashSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FAirDashTime; //how long until gravity kicks in
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BAirDashTime; //how long until gravity kicks in
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FAirDashNoAttackTime; //how long until airdash cancellable
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BAirDashNoAttackTime; //how long until airdash cancellable
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AirJumpCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AirDashCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StandPushWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StandPushHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CrouchPushWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CrouchPushHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AirPushWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AirPushHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AirPushHeightLow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CloseNormalRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ComboRate = 60;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ForwardWalkMeterGain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ForwardJumpMeterGain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ForwardDashMeterGain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ForwardAirDashMeterGain;
	UPROPERTY(EditAnywhere)
	TArray<FString> ThrowLockCels;
	
	UPROPERTY()
	FStateMachine StateMachine; //state machine
	FInputBuffer InputBuffer; //input buffer

	int32 ChainCancelOptionsInternal[CancelArraySize]; //chain cancels (copied from TArray to static array)
    int32 WhiffCancelOptionsInternal[CancelArraySize]; //whiff cancels (copied from TArray to static array)
	CString<64> StateName;
	CString<64> ExeStateName;
	
	HitAction ReceivedHitAction = HACT_None; //last received hit action. clear after read
	int ReceivedAttackLevel = -1; //last received attack level. clear after read
	
	UPROPERTY()
	APlayerCharacter* Enemy; //pointer to active enemy.

	UPROPERTY()
	ABattleActor* ChildBattleActors[32];
	
	int32 PlayerSyncEnd; //anything past here isn't saved or loaded for rollback

	UPROPERTY()
	TArray<USubroutine*> Subroutines;
	TArray<FString> SubroutineNames;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USubroutineData* CommonSubroutineData; //list of common subroutines
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USubroutineData* CharaSubroutineData; //list of chara/object subroutines
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStateDataAsset* StateDataAsset; //list of states
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStateDataAsset* ObjectStateDataAsset; //list of object states
	UPROPERTY(BlueprintReadWrite)
	TArray<UState*> ObjectStates; //array of object states
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> ObjectStateNames; //array of object states
	
	TArray<FString> ChainCancelOptions; //chain cancels (only set via blueprints)
	TArray<FString> WhiffCancelOptions; //whiff cancels (only set via blueprints)

	UPROPERTY(EditAnywhere)
	UParticleData* CommonParticleData; //common particle data asset. only set in the base bp
	UPROPERTY(EditAnywhere)
	UParticleData* ParticleData; //character specific particle data asset
	UPROPERTY(EditAnywhere)
	USoundData* CommonSoundData; //holds all common sound effects
	UPROPERTY(EditAnywhere)
	USoundData* SoundData; //character sounds
	UPROPERTY(EditAnywhere)
	USoundData* VoiceData; //voices
	UPROPERTY(EditAnywhere)
	USequenceData* CommonSequenceData;
	UPROPERTY(EditAnywhere)
	USequenceData* SequenceData;

private:
	//internal functions
	virtual void BeginPlay() override;
	void HandleStateMachine(); //update state
	bool HandleStateCondition(EStateCondition StateCondition); //check state conditions
	bool FindChainCancelOption(FString Name); //check if chain cancel option exists
	bool FindWhiffCancelOption(FString Name); //check if whiff cancel option exists
	
public:
	virtual void Tick(float DeltaTime) override;
	void InitPlayer(); //initialize player for match/round start
	void HandleHitAction(); //based on received hit action, choose state
	bool IsCorrectBlock(EBlockType BlockType); //check attack against block stance
	void HandleBlockAction(); //jump to correct block state
	void OnStateChange(); //called whenever state changes
	virtual void Update() override;
	void SaveForRollbackPlayer(unsigned char* Buffer);
	void LoadForRollbackPlayer(unsigned char* Buffer);
	virtual void LogForSyncTest() override;
	virtual void LogForSyncTestFile(FILE* file) override;
	void ThrowExe(); //upon successful throw, jump to state
	void HandleThrowCollision(); //handles throwing objects
	bool CheckKaraCancel(EStateType InStateType);
	bool CheckObjectPreventingState(int InObjectID);
	void ResetForRound();
	void HandleWallBounce();
	void HandleGroundBounce();
	
	//bp callable functions
	UFUNCTION(BlueprintCallable)
	void AddState(FString Name, UState* State); //add state to state machine
	UFUNCTION(BlueprintCallable)
	void AddSubroutine(FString Name, USubroutine* Subroutine); //add state to state machine
	UFUNCTION(BlueprintCallable)
	void CallSubroutine(FString Name);
	UFUNCTION(BlueprintCallable)
	void UseMeter(int Use);
	UFUNCTION(BlueprintCallable)
	void AddMeter(int Meter);
	UFUNCTION(BlueprintCallable)
	void SetActionFlags(EActionFlags ActionFlag); //set standing/crouching/jumping
	UFUNCTION(BlueprintCallable)
	void JumpToState(FString NewName); //force set state
	UFUNCTION(BlueprintPure)
	FString GetCurrentStateName(); //gets current state name
	UFUNCTION(BlueprintPure)
	int32 GetLoopCount(); //gets loop counter
	UFUNCTION(BlueprintCallable)
	void IncrementLoopCount(); //gets loop counter
	UFUNCTION(BlueprintCallable)
	bool CheckStateEnabled(EStateType StateType); //check if state can be entered
	UFUNCTION(BlueprintCallable)
	void EnableState(EEnableFlags NewEnableFlags); //enable state type
	UFUNCTION(BlueprintCallable)
	void EnableAttacks(); //enable all attacks only
	UFUNCTION(BlueprintCallable)
	void DisableState(EEnableFlags NewEnableFlags); //disable state type
	UFUNCTION(BlueprintCallable)
	void DisableGroundMovement(); //disable ground movement only
	UFUNCTION(BlueprintCallable)
	void EnableAll(); //enable all states (besides tech)
	UFUNCTION(BlueprintCallable)
	void DisableAll(); //disable all states (besides tech)
	UFUNCTION(BlueprintPure)
	bool CheckInputRaw(EInputFlags Input); //checks raw inputs (after side switching)
	UFUNCTION(BlueprintPure)
	bool CheckInput(EInputCondition Input); //checks input condition
	UFUNCTION(BlueprintPure)
	bool CheckIsStunned();
	UFUNCTION(BlueprintCallable)
	void AddAirJump(int32 NewAirJump); //temporarily adds air jump
	UFUNCTION(BlueprintCallable)
	void AddAirDash(int32 NewAirDash); //temporarily adds air dash
	UFUNCTION(BlueprintCallable)
	void SetAirDashTimer(bool IsForward); //set air dash timer (set is forward for forward airdashes)
	UFUNCTION(BlueprintCallable)
	void AddChainCancelOption(FString Option); //add chain cancel option, use this in OnEntry
	UFUNCTION(BlueprintCallable)
	void AddWhiffCancelOption(FString Option); //add whiff cancel option, use this in OnEntry
	UFUNCTION(BlueprintCallable)
	void EnableJumpCancel(bool Enable); //sets jump cancel on hit enabled
	UFUNCTION(BlueprintCallable)
	void EnableFAirDashCancel(bool Enable); //sets forward air dash cancel on hit enabled
	UFUNCTION(BlueprintCallable)
	void EnableBAirDashCancel(bool Enable); //sets back air dash cancel on hit enabled
	UFUNCTION(BlueprintCallable)
	void EnableChainCancel(bool Enable); //sets chain cancel options enabled. on by default 
	UFUNCTION(BlueprintCallable)
	void EnableWhiffCancel(bool Enable); //sets whiff cancel options enabled. off by default
	UFUNCTION(BlueprintCallable)
	void EnableSpecialCancel(bool Enable); //sets special cancel enabled. off by default
	UFUNCTION(BlueprintCallable)
	void EnableSuperCancel(bool Enable); //sets super cancel enabled. off by default
	UFUNCTION(BlueprintCallable)
	void SetDefaultLandingAction(bool Enable); //toggles default landing action. if true, landing will go to JumpLanding state. if false, define your own landing.
	UFUNCTION(BlueprintCallable)
	void SetStrikeInvulnerable(bool Invulnerable); //sets strike invulnerable enabled
	UFUNCTION(BlueprintCallable)
	void SetThrowInvulnerable(bool Invulnerable); //sets throw invulnerable enabled
	UFUNCTION(BlueprintCallable)
	void SetHeadInvulnerable(bool Invulnerable); //sets throw invulnerable enabled
	UFUNCTION(BlueprintCallable)
	void ForceEnableFarNormal(bool Enable);
	UFUNCTION(BlueprintCallable)
	void SetThrowActive(bool Active); //initiate throw
	UFUNCTION(BlueprintCallable)
	void ThrowEnd(); //end throw
	UFUNCTION(BlueprintCallable)
	void SetThrowRange(int32 InThrowRange); //initiate throw range
	UFUNCTION(BlueprintCallable)
	void SetThrowExeState(FString ExeState); //initiate throw range
	UFUNCTION(BlueprintCallable)
	void SetThrowPosition(int32 ThrowPosX, int32 ThrowPosY); //initiate throw range
	UFUNCTION(BlueprintCallable)
	void PlayVoice(FString Name);
	UFUNCTION(BlueprintCallable)
    void PlayCommonLevelSequence(FString Name);
	UFUNCTION(BlueprintCallable)
    void PlayLevelSequence(FString Name);
	UFUNCTION(BlueprintCallable)
	void StartSuperFreeze(int Duration);
	UFUNCTION(BlueprintCallable)
	void BattleHudVisibility(bool Visible);
	UFUNCTION(BlueprintCallable)
	void SpaceInputBuffer();
	UFUNCTION(BlueprintCallable)
	void AddBattleActor(FString InStateName, int32 PosXOffset, int32 PosYOffset); //creates object
	
	UFUNCTION(BlueprintImplementableEvent)
	void Init(); //only use this in BP_BaseCharacter to initialize the state machine
};

#define SIZEOF_PLAYERCHARACTER offsetof(APlayerCharacter, PlayerSyncEnd) - offsetof(APlayerCharacter, PlayerSync)
#pragma pack(pop)
