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
constexpr int MaxComponentCount = 80;

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API APlayerCharacter : public ABattleActor
{
	GENERATED_BODY()
public:
	APlayerCharacter();

	//starting from this until PlayerSyncEnd, everything is saved/loaded for rollback
	unsigned char PlayerSync; 
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
	bool EnableKaraCancel = true;
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
	bool IsKnockedDown;
	UPROPERTY(BlueprintReadWrite)
	bool FlipInputs;
	int32 TeamIndex;
	int32 Inputs;
	int32 ActionFlags;
	int32 AirDashTimer;
	int32 AirDashNoAttackTime;
	UPROPERTY(BlueprintReadOnly)
	int32 PlayerIndex;
	int32 Hitstun = -1;
	int32 Blockstun = -1;
	int32 Untech = -1;
	int32 KnockdownTime = -1;
	int32 TotalProration = 10000;
	int32 ComboCounter = 0;
	int32 ComboTimer = 0;
	int32 LoopCounter = 0;
	int32 ThrowTechTimer;
	int32 HasBeenOTG;
	int32 WallTouchTimer;
	bool TouchingWall;
	bool ChainCancelEnabled = true;
	bool WhiffCancelEnabled;
	bool StrikeInvulnerable;
	bool ThrowInvulnerable;
	bool ProjectileInvulnerable;
	bool HeadInvulnerable;
	int RoundWinTimer = 300;
	bool RoundWinInputLock;
	int MeterCooldownTimer = 0;
	
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
	//how long until gravity kicks in
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FAirDashTime;
	//how long until gravity kicks in
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BAirDashTime;
	//how long until airdash cancellable
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FAirDashNoAttackTime;
	//how long until airdash cancellable
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BAirDashNoAttackTime; 
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MeterPercentOnHit = 72;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MeterPercentOnHitGuard = 18;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MeterPercentOnReceiveHitGuard = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MeterPercentOnReceiveHit = 40;
	UPROPERTY(EditAnywhere)
	TArray<FString> ThrowLockCels;

	//custom player values, reset at end of round. use to keep track of values (timers, toggles, counters, etc) that are specific to the character.
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerVal1 = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerVal2 = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerVal3 = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerVal4 = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerVal5 = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerVal6 = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerVal7 = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerVal8 = 0;
	
	//state machine
	UPROPERTY()
	FStateMachine StateMachine;
	//input buffer
	FInputBuffer InputBuffer; 

	//chain cancels (copied from TArray to static array)
	int32 ChainCancelOptionsInternal[CancelArraySize];
	//whiff cancels (copied from TArray to static array)
    int32 WhiffCancelOptionsInternal[CancelArraySize]; 
	CString<64> BufferedStateName;
	CString<64> StateName;
	CString<64> ExeStateName;

	//last received hit action. clear after read
	HitAction ReceivedHitAction = HACT_None;
	//last received attack level. clear after read
	int ReceivedAttackLevel = -1; 

	//pointer to active enemy.
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* Enemy; 

	UPROPERTY()
	ABattleActor* ChildBattleActors[32];

	UPROPERTY()
	ABattleActor* StoredBattleActors[16];

	bool ComponentVisible[MaxComponentCount];
	
	//anything past here isn't saved or loaded for rollback	
	int32 PlayerSyncEnd; 

	UPROPERTY()
	TArray<USubroutine*> Subroutines;
	TArray<FString> SubroutineNames;

	//list of common subroutines
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USubroutineData* CommonSubroutineData;
	//list of chara/object subroutines
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USubroutineData* CharaSubroutineData;
	//list of states
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStateDataAsset* StateDataAsset;
	//list of object states
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStateDataAsset* ObjectStateDataAsset;
	//list of object states
	UPROPERTY(BlueprintReadWrite)
	TArray<UState*> ObjectStates;
	//array of object states
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> ObjectStateNames; 

	//chain cancels (only set via blueprints)
	TArray<FString> ChainCancelOptions;
	//whiff cancels (only set via blueprints)
	TArray<FString> WhiffCancelOptions; 

	//common particle data asset. only set in the base bp
	UPROPERTY(EditAnywhere)
	UParticleData* CommonParticleData; 
	//character specific particle data asset
	UPROPERTY(EditAnywhere)
	UParticleData* ParticleData;
	//holds all common sound effects
	UPROPERTY(EditAnywhere)
	USoundData* CommonSoundData;
	//character sounds
	UPROPERTY(EditAnywhere)
	USoundData* SoundData;
	//voices
	UPROPERTY(EditAnywhere)
	USoundData* VoiceData; 
	//common level sequences
	UPROPERTY(EditAnywhere)
	USequenceData* CommonSequenceData;
	//character level sequences
	UPROPERTY(EditAnywhere)
	USequenceData* SequenceData;
	UPROPERTY(BlueprintReadOnly)
	int32 ColorIndex = 1;
	
private:
	//internal functions
	virtual void BeginPlay() override;
	//update state
	void HandleStateMachine(bool Buffer);
	//buffer state
	void HandleBufferedState();
	//check state conditions
	bool HandleStateCondition(EStateCondition StateCondition);
	//check if chain cancel option exists
	bool FindChainCancelOption(FString Name);
	//check if whiff cancel option exists
	bool FindWhiffCancelOption(FString Name); 
	//sets visibility of components
	void SetComponentVisibility();
	
public:
	virtual void Tick(float DeltaTime) override;
	//initialize player for match/round start
	void InitPlayer();
	//based on received hit action, choose state
	void HandleHitAction();
	//check attack against block stance
	bool IsCorrectBlock(EBlockType BlockType);
	//jump to correct block state
	void HandleBlockAction(EBlockType BlockType);
	//called whenever state changes
	void OnStateChange(); 
	virtual void Update() override;
	void SaveForRollbackPlayer(unsigned char* Buffer);
	void LoadForRollbackPlayer(unsigned char* Buffer);
	virtual void LogForSyncTest() override;
	virtual void LogForSyncTestFile(FILE* file) override;
	//upon successful throw, jump to state
	void ThrowExe();
	//handles throwing objects
	void HandleThrowCollision();
	//checks kara cancel
	bool CheckKaraCancel(EStateType InStateType);
	//checks if a child object with a corresponding object id exists. if so, do not enter state 
	bool CheckObjectPreventingState(int InObjectID);
	//resets object for next round
	void ResetForRound();
	//handles wall bounce
	void HandleWallBounce();
	//handles ground bounce
	void HandleGroundBounce();
	
	//bp callable functions
	//add state to state machine
	UFUNCTION(BlueprintCallable)
	void AddState(FString Name, UState* State); 
	//add subroutine to state machine
	UFUNCTION(BlueprintCallable)
	void AddSubroutine(FString Name, USubroutine* Subroutine);
	//calls subroutine
	UFUNCTION(BlueprintCallable)
	void CallSubroutine(FString Name);
	//use meter
	UFUNCTION(BlueprintCallable)
	void UseMeter(int Use);
	//add meter
	UFUNCTION(BlueprintCallable)
	void AddMeter(int Meter);
	//sets meter gain cooldoown timer
	UFUNCTION(BlueprintCallable)
	void SetMeterCooldownTimer(int Timer);
	//set standing/crouching/jumping
	UFUNCTION(BlueprintCallable)
	void SetActionFlags(EActionFlags ActionFlag);
	//force set state
	UFUNCTION(BlueprintCallable, CallInEditor)
	void JumpToState(FString NewName);
	//gets current state name
	UFUNCTION(BlueprintPure)
	FString GetCurrentStateName();
	//gets loop counter
	UFUNCTION(BlueprintPure)
	int32 GetLoopCount();
	//gets loop counter
	UFUNCTION(BlueprintCallable)
	void IncrementLoopCount();
	//check if state can be entered
	UFUNCTION(BlueprintCallable)
	bool CheckStateEnabled(EStateType StateType);
	//enable state type
	UFUNCTION(BlueprintCallable)
	void EnableState(EEnableFlags NewEnableFlags);
	//enable all attacks only
	UFUNCTION(BlueprintCallable)
	void EnableAttacks();
	//disable state type
	UFUNCTION(BlueprintCallable)
	void DisableState(EEnableFlags NewEnableFlags);
	//disable ground movement only
	UFUNCTION(BlueprintCallable)
	void DisableGroundMovement();
	//enable all states (besides tech)
	UFUNCTION(BlueprintCallable)
	void EnableAll(); 
	//disable all states (besides tech)
	UFUNCTION(BlueprintCallable)
	void DisableAll();
	//checks raw inputs (after side switching)
	UFUNCTION(BlueprintPure)
	bool CheckInputRaw(EInputFlags Input);
	//checks input condition
	UFUNCTION(BlueprintPure)
	bool CheckInput(FInputCondition Input); 
	UFUNCTION(BlueprintPure)
	bool CheckIsStunned();
	//temporarily adds air jump
	UFUNCTION(BlueprintCallable)
	void AddAirJump(int32 NewAirJump);
	//temporarily adds air dash
	UFUNCTION(BlueprintCallable)
	void AddAirDash(int32 NewAirDash);
	//set air dash timer (set is forward for forward airdashes)
	UFUNCTION(BlueprintCallable)
	void SetAirDashTimer(bool IsForward);
	//add chain cancel option, use this in OnEntry
	UFUNCTION(BlueprintCallable)
	void AddChainCancelOption(FString Option);
	//add whiff cancel option, use this in OnEntry
	UFUNCTION(BlueprintCallable)
	void AddWhiffCancelOption(FString Option);
	//sets jump cancel on hit enabled
	UFUNCTION(BlueprintCallable)
	void EnableJumpCancel(bool Enable);
	//sets forward air dash cancel on hit enabled
	UFUNCTION(BlueprintCallable)
	void EnableFAirDashCancel(bool Enable);
	//sets back air dash cancel on hit enabled
	UFUNCTION(BlueprintCallable)
	void EnableBAirDashCancel(bool Enable);
	//sets chain cancel options enabled. on by default 
	UFUNCTION(BlueprintCallable)
	void EnableChainCancel(bool Enable);
	//sets whiff cancel options enabled. off by default
	UFUNCTION(BlueprintCallable)
	void EnableWhiffCancel(bool Enable);
	//sets special cancel enabled. off by default
	UFUNCTION(BlueprintCallable)
	void EnableSpecialCancel(bool Enable);
	//sets super cancel enabled. off by default
	UFUNCTION(BlueprintCallable)
	void EnableSuperCancel(bool Enable);
	//toggles default landing action. if true, landing will go to JumpLanding state. if false, define your own landing.
	UFUNCTION(BlueprintCallable)
	void SetDefaultLandingAction(bool Enable);
	//sets strike invulnerable enabled
	UFUNCTION(BlueprintCallable)
	void SetStrikeInvulnerable(bool Invulnerable);
	//sets throw invulnerable enabled
	UFUNCTION(BlueprintCallable)
	void SetThrowInvulnerable(bool Invulnerable);
	//sets projectile invulnerable enabled
	UFUNCTION(BlueprintCallable)
	void SetProjectileInvulnerable(bool Invulnerable);
	//sets head attribute invulnerable enabled
	UFUNCTION(BlueprintCallable)
	void SetHeadInvulnerable(bool Invulnerable);
	//force enables far proximity normals
	UFUNCTION(BlueprintCallable)
	void ForceEnableFarNormal(bool Enable);
	//initiate throw
	UFUNCTION(BlueprintCallable)
	void SetThrowActive(bool Active);
	//end throw
	UFUNCTION(BlueprintCallable)
	void ThrowEnd(); 
	//initiate throw range
	UFUNCTION(BlueprintCallable)
	void SetThrowRange(int32 InThrowRange);
	//sets throw execution state
	UFUNCTION(BlueprintCallable)
	void SetThrowExeState(FString ExeState);
	//sets grip position for throw
	UFUNCTION(BlueprintCallable)
	void SetThrowPosition(int32 ThrowPosX, int32 ThrowPosY);
	//sets grip position for throw
	UFUNCTION(BlueprintCallable)
	void SetThrowLockCel(int32 Index);
	//plays voice line
	UFUNCTION(BlueprintCallable)
	void PlayVoice(FString Name);
	//plays common level sequence
	UFUNCTION(BlueprintCallable)
    void PlayCommonLevelSequence(FString Name);
	//plays character level sequence
	UFUNCTION(BlueprintCallable)
    void PlayLevelSequence(FString Name);
	//starts super freeze
	UFUNCTION(BlueprintCallable)
	void StartSuperFreeze(int Duration);
	//toggles hud visibility
	UFUNCTION(BlueprintCallable)
	void BattleHudVisibility(bool Visible);
	//disables last input
	UFUNCTION(BlueprintCallable)
	void DisableLastInput();
	//creates object
	UFUNCTION(BlueprintCallable)
	ABattleActor* AddBattleActor(FString InStateName, int32 PosXOffset, int32 PosYOffset);
	//stores battle actor in slot
	UFUNCTION(BlueprintCallable)
	void AddBattleActorToStorage(ABattleActor* InActor, int Index);
    UFUNCTION(BlueprintCallable)
    void ToggleComponentVisibility(FString ComponentName, bool Visible);

	//ONLY CALL AT THE START OF InitStateMachine! OTHERWISE THE GAME WILL CRASH
	UFUNCTION(BlueprintCallable)
	void EmptyStateMachine();

#if WITH_EDITOR
	//updates the state machine for the editor
	UFUNCTION(BlueprintCallable)
	void EditorUpdate();
#endif
};

#define SIZEOF_PLAYERCHARACTER offsetof(APlayerCharacter, PlayerSyncEnd) - offsetof(APlayerCharacter, PlayerSync)
#pragma pack(pop)
