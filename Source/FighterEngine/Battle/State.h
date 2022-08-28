// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	Input_1_Press,
	Input_1_Hold,
	Input_1_Release,
	Input_2_Press,
	Input_2_Hold,
	Input_2_Release,
	Input_3_Press,
	Input_3_Hold,
	Input_3_Release,
	Input_4_Press,
	Input_4_Hold,
	Input_4_Release,
	Input_6_Press,
	Input_6_Hold,
	Input_6_Release,
	Input_7_Press,
	Input_7_Hold,
	Input_7_Release,
	Input_8_Press,
	Input_8_Hold,
	Input_8_Release,
	Input_9_Press,
	Input_9_Hold,
	Input_9_Release,
	Input_Down_Press,
	Input_Down_Hold,
	Input_Down_Release,
	Input_Left_Press,
	Input_Left_Hold,
	Input_Left_Release,
	Input_Right_Press,
	Input_Right_Hold,
	Input_Right_Release,
	Input_Up_Press,
	Input_Up_Hold,
	Input_Up_Release,
	Input_SuperJump,
	Input_SuperJump_Back,
	Input_SuperJump_Forward,
	Input_44,
	Input_66,
	Input_236,
	Input_214,
	Input_L_Press,
	Input_L_Hold,
	Input_L_Release,
	Input_M_Press,
	Input_M_Hold,
	Input_M_Release,
	Input_H_Press,
	Input_H_Hold,
	Input_H_Release,
	Input_S_Press,
	Input_S_Hold,
	Input_S_Release,
	Input_L_And_S,
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
};

UENUM(BlueprintType)
enum class EStateCondition : uint8
{
	None,
	AirJumpOk,
	AirDashOk,
	AirActionMinimumHeight,
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
	TArray<EInputCondition> InputConditions;
	UPROPERTY(EditAnywhere)
	EStateType StateType;
	UPROPERTY(EditAnywhere)
	TArray<EStateCondition> StateConditions;
	UPROPERTY(EditAnywhere)
	bool IsFollowupState;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnEnter(); //executes on enter. write in bp
	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdate(float DeltaTime); //executes every frame. write in bp
	UFUNCTION(BlueprintImplementableEvent)
	void OnExit(); //executes on exit. write in bp
	UFUNCTION(BlueprintImplementableEvent)
	void OnLanding(); //executes on landing. write in bp
	UFUNCTION(BlueprintImplementableEvent)
	void OnHit(); //executes on hit. write in bp
	UFUNCTION(BlueprintImplementableEvent)
    void OnCounterHit(); //executes on counter hit. write in bp
	UFUNCTION(BlueprintImplementableEvent)
	void OnSuperFreeze(); //executes on super freeze. write in bp
};
