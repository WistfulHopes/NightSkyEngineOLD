﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bitflags.h"
#include "State.generated.h"

class APlayerCharacter;
class ABattleActor;

UENUM(BlueprintType)
enum class EEntryState : uint8 //character state required to enter state
{
	None, //any
	Standing,
	Jumping,
	Crouching,
};

UENUM(BlueprintType)
enum class EInputCondition : uint8 //input required to enter state
{
	None, //no input
	Input_1,
	Input_1_Press,
	Input_1_Hold,
	Input_1_Release,
	Input_2,
	Input_2_Press,
	Input_2_Hold,
	Input_2_Release,
	Input_3,
	Input_3_Press,
	Input_3_Hold,
	Input_3_Release,
	Input_4,
	Input_4_Press,
	Input_4_Hold,
	Input_4_Release,
	Input_6,
	Input_6_Press,
	Input_6_Hold,
	Input_6_Release,
	Input_7,
	Input_7_Press,
	Input_7_Hold,
	Input_7_Release,
	Input_8,
	Input_8_Press,
	Input_8_Hold,
	Input_8_Release,
	Input_9,
	Input_9_Press,
	Input_9_Hold,
	Input_9_Release,
	Input_Down,
	Input_Down_Press,
	Input_Down_Hold,
	Input_Down_Release,
	Input_Left,
	Input_Left_Press,
	Input_Left_Hold,
	Input_Left_Release,
	Input_Right,
	Input_Right_Press,
	Input_Right_Hold,
	Input_Right_Release,
	Input_Up,
	Input_Up_Press,
	Input_Up_Hold,
	Input_Up_Release,
	Input_SuperJump,
	Input_SuperJump_Back,
	Input_SuperJump_Forward,
	Input_DownDown,
	Input_44,
	Input_66,
	Input_236,
	Input_214,
	Input_623,
	Input_421,
	Input_41236,
	Input_63214,
	Input_236236,
	Input_214214,
	Input_L,
	Input_L_Press,
	Input_L_Hold,
	Input_L_Release,
	Input_M,
	Input_M_Press,
	Input_M_Hold,
	Input_M_Release,
	Input_H,
	Input_H_Press,
	Input_H_Hold,
	Input_H_Release,
	Input_S,
	Input_S_Press,
	Input_S_Hold,
	Input_S_Release,
	Input_L_And_S,
	Input_Any_LMHS,
};

UENUM(BlueprintType)
enum class EStateType : uint8
{
	Standing,
	Crouching,
	NeutralJump,
	ForwardJump,
	BackwardJump,
	ForwardWalk,
	BackwardWalk,
	ForwardDash,
	BackwardDash,
	ForwardAirDash,
	BackwardAirDash,
	NormalAttack,
	NormalThrow,
	SpecialAttack,
	SuperAttack,
	Hitstun,
	Blockstun,
	Tech,
	Parry,
	Burst,
};

UENUM(BlueprintType)
enum class EStateCondition : uint8
{
	None,
	AirJumpOk,
	AirJumpMinimumHeight,
	AirDashOk,
	AirDashMinimumHeight,
	IsAttacking,
	HitstopCancel,
	IsStunned,
	CloseNormal,
	FarNormal,
	MeterNotZero,
	MeterQuarterBar,
	MeterHalfBar,
	MeterOneBar,
	MeterTwoBars,
	MeterThreeBars,
	MeterFourBars,
	MeterFiveBars,
	UniversalGaugeOneBar,
	UniversalGaugeTwoBars,
	UniversalGaugeThreeBars,
	UniversalGaugeFourBars,
	UniversalGaugeFiveBars,
	UniversalGaugeSixBars,
};

UENUM()
enum class EInputMethod : uint8
{
	Normal,
	Strict,
	Once,
	OnceStrict,
};

USTRUCT(BlueprintType)
struct FInputBitmask
{
	GENERATED_BODY()

	FInputBitmask()
	{
		InputFlag = InputNone;
	};
	FInputBitmask(EInputFlags Input)
	{
		InputFlag = Input;
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = EInputFlags))
	int InputFlag;
};

USTRUCT(BlueprintType)
struct FInputCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInputBitmask> Sequence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Lenience = 8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ImpreciseInputCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInputAllowDisable = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInputMethod Method = EInputMethod::Normal;
};

USTRUCT()
struct FInputConditionList
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<FInputCondition> InputConditions;
};


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FIGHTERENGINE_API UState : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* Parent;
	UPROPERTY(BlueprintReadOnly)
	ABattleActor* ObjectParent;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Name;
	UPROPERTY(EditAnywhere)
	EEntryState EntryState;
	UPROPERTY(EditAnywhere)
	TArray<FInputConditionList> InputConditionList;
	UPROPERTY(EditAnywhere)
	EStateType StateType;
	UPROPERTY(EditAnywhere)
	TArray<EStateCondition> StateConditions;
	UPROPERTY(EditAnywhere)
	bool IsFollowupState;
	UPROPERTY(EditAnywhere)
	int ObjectID;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnEnter(); //executes on enter. write in bp
	UFUNCTION(BlueprintNativeEvent)
	void OnUpdate(float DeltaTime); //executes every frame. write in bp
	UFUNCTION(BlueprintNativeEvent)
	void OnExit(); //executes on exit. write in bp
	UFUNCTION(BlueprintNativeEvent)
	void OnLanding(); //executes on landing. write in bp
	UFUNCTION(BlueprintNativeEvent)
	void OnHit(); //executes on hit. write in bp
	UFUNCTION(BlueprintNativeEvent)
	void OnBlock(); //executes on hit. write in bp
	UFUNCTION(BlueprintNativeEvent)
	void OnHitOrBlock(); //executes on hit. write in bp
	UFUNCTION(BlueprintNativeEvent)
    void OnCounterHit(); //executes on counter hit. write in bp
	UFUNCTION(BlueprintNativeEvent)
	void OnSuperFreeze(); //executes on super freeze. write in bp
	UFUNCTION(BlueprintNativeEvent)
	void OnSuperFreezeEnd(); //executes on super freeze. write in bp
};
