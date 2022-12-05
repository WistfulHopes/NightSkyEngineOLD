// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleActor.h"
#include "FighterEngine/DataAssets/SequenceData.h"
#include "FighterEngine/DataAssets/SoundData.h"
#include "FighterEngine/DataAssets/StateDataAsset.h"
#include "FighterEngine/DataAssets/ParticleData.h"
#include "FighterEngine/UnrealBattle/BlueprintState.h"
#include "FighterEngine/UnrealBattle/BlueprintSubroutine.h"
#include "FighterEngine/UnrealBattle/Bitflags.h"
#include "FighterEngine/DataAssets/SubroutineData.h"
#include "PlayerCharacter.generated.h"

class PlayerCharacter;

/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API APlayerCharacter : public ABattleActor
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	
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

	//pointer to active enemy.
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* Enemy; 

	UPROPERTY()
	//array of common subroutines
	TArray<USubroutine*> CommonSubroutines;
	//array of common subroutines
	TArray<FString> CommonSubroutineNames;
	
	UPROPERTY()
	//array of subroutines
	TArray<USubroutine*> Subroutines;
	//array of subroutine names
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
	//array of chara states
	UPROPERTY(BlueprintReadWrite)
	TArray<UState*> States;
	//array of chara state names
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> StateNames; 
	//array of object states
	UPROPERTY(BlueprintReadWrite)
	TArray<UState*> ObjectStates;
	//array of object names
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

	//internal functions
	virtual void Init();
	virtual void Update() override;
	virtual void OnLoadGameState() override;
	void SetParent(PlayerCharacter* InActor);

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
	bool CheckInput(EInputCondition Input); 
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
	//creates object
	UFUNCTION(BlueprintCallable)
	ABattleActor* AddBattleActor(FString InStateName, int32 PosXOffset, int32 PosYOffset);
	//stores battle actor in slot
	UFUNCTION(BlueprintCallable)
	void AddBattleActorToStorage(ABattleActor* InActor, int Index);

	UFUNCTION(BlueprintImplementableEvent)
	void InitStateMachine();

#if WITH_EDITOR
	//updates the state machine for the editor.
	UFUNCTION(BlueprintCallable)
	void EditorUpdate();
#endif
};