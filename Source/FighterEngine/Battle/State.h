// Fill out your copyright notice in the Description page of Project Settings.

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
	Crouching,
	Jumping,
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
	PlayerVal1True,
	PlayerVal2True,
	PlayerVal3True,
	PlayerVal4True,
	PlayerVal5True,
	PlayerVal6True,
	PlayerVal7True,
	PlayerVal8True,
	PlayerVal1False,
	PlayerVal2False,
	PlayerVal3False,
	PlayerVal4False,
	PlayerVal5False,
	PlayerVal6False,
	PlayerVal7False,
	PlayerVal8False,
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
