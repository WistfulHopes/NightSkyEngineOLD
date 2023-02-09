// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppHidingFunction
#pragma once

#include "CoreMinimal.h"
#include "FighterEngine/Battle/State.h"
#include "FighterEngine/Battle/Subroutine.h"
#include "FighterEngine/Miscellaneous/CString.h"
#include "ScriptAnalyzer.generated.h"

UENUM()
enum EOpCodes
{
	OPC_BeginState = 0,
	OPC_EndState = 1,
	OPC_SetCel = 2,
	OPC_BeginSubroutine = 3,
	OPC_EndSubroutine = 4,
	OPC_CallSubroutine = 5,
	OPC_CallSubroutineWithArgs = 6,
	OPC_ExitState = 7,
	OPC_JumpToState = 8,
	OPC_IsOnFrame = 9,
	OPC_EndBlock = 10,
	OPC_OnEnter = 11,
	OPC_OnUpdate = 12,
	OPC_OnExit = 13,
	OPC_OnLanding = 14,
	OPC_OnHit = 15,
	OPC_OnBlock = 16,
	OPC_OnHitOrBlock = 17,
	OPC_OnCounterHit = 18,
	OPC_OnSuperFreeze = 19,
	OPC_OnSuperFreezeEnd = 20,
	OPC_BeginLabel = 50,
	OPC_EndLabel = 51,
	OPC_GotoLabel = 52,
	OPC_If = 53,
	OPC_EndIf = 54,
	OPC_IfOperation = 55,
	OPC_IfNot = 56,
	OPC_Else = 57,
	OPC_EndElse = 58,
	OPC_GotoLabelIf = 59,
	OPC_GotoLabelIfOperation = 60,
	OPC_GotoLabelIfNot = 61,
	OPC_GetPlayerStats = 70,
	OPC_ModifyInternalValue = 75,
	OPC_StoreInternalValue = 76,
	OPC_ModifyInternalValueAndSave = 77,
	OPC_SetPosX = 100,
	OPC_AddPosX = 101,
	OPC_AddPosXRaw = 102,
	OPC_SetPosY = 105,
	OPC_AddPosY = 106,
	OPC_SetSpeedX = 109,
	OPC_AddSpeedX = 110,
	OPC_SetSpeedY = 111,
	OPC_AddSpeedY = 112,
	OPC_SetSpeedXPercent = 113,
	OPC_SetSpeedXPercentPerFrame = 114,
	OPC_SetSpeedYPercent = 115,
	OPC_SetSpeedYPercentPerFrame = 116,
	OPC_SetGravity = 120,
	OPC_AddGravity = 121,
	OPC_SetInertia = 125,
	OPC_AddInertia = 126,
	OPC_EnableInertia = 127,
	OPC_HaltMomentum = 150,
	OPC_ClearInertia = 151,
	OPC_SetActionFlags = 200,
	OPC_CreateParticle = 400,
	OPC_AddBattleActor = 401,
	OPC_AddAirJump = 550,
	OPC_AddAirDash = 551,
	OPC_SetAirDashTimer = 554,
	OPC_CheckInput = 900,
	OPC_CheckInputRaw = 901,
	OPC_MakeInput = 910,
	OPC_MakeInputSequenceBitmask = 911,
	OPC_MakeInputLenience = 912,
	OPC_MakeInputImpreciseCount = 913,
	OPC_MakeInputAllowDisable = 914,
	OPC_MakeInputMethod = 915,
	OPC_BeginStateDefine = 1000,
	OPC_EndStateDefine = 1001,
	OPC_SetStateType = 1002,
	OPC_SetEntryState = 1003,
	OPC_AddInputCondition = 1004,
	OPC_AddInputConditionList = 1005,
	OPC_AddStateCondition = 1006,
	OPC_IsFollowupMove = 1007,
	OPC_SetStateObjectID = 1008,
	OPC_SetParentState = 1009,
	OPC_EnableState = 1200,
	OPC_DisableState = 1201,
	OPC_EnableAll = 1202,
	OPC_DisableAll = 1203,
	OPC_EnableFlip = 1204,
	OPC_ForceEnableFarNormal = 1205,
};

enum EScriptOperation
{
	OP_Add = 0,
	OP_Sub = 1,
	OP_Mul = 2,
	OP_Div = 3,
	OP_Mod = 4,
	OP_And = 5,
	OP_Or = 6,
	OP_BitwiseAnd = 7,
	OP_BitwiseOr = 8,
	OP_IsEqual = 9,
	OP_IsGreater = 10,
	OP_IsLesser = 11,
	OP_IsGreaterOrEqual = 12,
	OP_IsLesserOrEqual = 13,
	OP_BitDelete = 14,
	OP_IsNotEqual = 15,
};

static TMap<EOpCodes, uint32> OpCodeSizes = {
	{OPC_BeginState, 68},
	{OPC_EndState, 4},
	{OPC_SetCel, 72},
	{OPC_BeginSubroutine, 68},
	{OPC_EndSubroutine, 4},
	{OPC_CallSubroutine, 68},
	{OPC_CallSubroutineWithArgs, 100},
	{OPC_ExitState, 4},
	{OPC_JumpToState, 68},
	{OPC_IsOnFrame, 12},
	{OPC_EndBlock, 4},
	{OPC_OnEnter, 4},
	{OPC_OnUpdate, 4},
	{OPC_OnExit, 4},
	{OPC_OnLanding, 4},
	{OPC_OnHit, 4},
	{OPC_OnBlock, 4},
	{OPC_OnHitOrBlock, 4},
	{OPC_OnCounterHit, 4},
	{OPC_OnSuperFreeze, 4},
	{OPC_OnSuperFreezeEnd, 4},
	{OPC_BeginLabel, 68},
	{OPC_EndLabel, 8},
	{OPC_GotoLabel, 68},
	{OPC_If, 12},
	{OPC_EndIf,4},
	{OPC_IfOperation, 24},
	{OPC_IfNot, 12},
	{OPC_Else, 4},
	{OPC_EndElse, 4},
	{OPC_GotoLabelIf, 76},
	{OPC_GotoLabelIfOperation, 88},
	{OPC_GotoLabelIfNot, 76},
	{OPC_GetPlayerStats, 8},
	{OPC_ModifyInternalValue, 24},
	{OPC_StoreInternalValue, 20},
	{OPC_ModifyInternalValueAndSave, 32},
	{OPC_SetPosX, 12},
	{OPC_AddPosX, 12},
	{OPC_AddPosXRaw, 12},
	{OPC_SetPosY, 12},
	{OPC_AddPosY, 12},
	{OPC_SetSpeedX, 12},
	{OPC_AddSpeedX, 12},
	{OPC_SetSpeedY, 12},
	{OPC_AddSpeedY, 12},
	{OPC_SetSpeedXPercent, 12},
	{OPC_SetSpeedXPercentPerFrame, 12},
	{OPC_SetSpeedYPercent, 12},
	{OPC_SetSpeedYPercentPerFrame, 12},
	{OPC_SetGravity, 12},
	{OPC_AddGravity, 12},
	{OPC_SetInertia, 12},
	{OPC_AddInertia, 12},
	{OPC_EnableInertia, 8},
	{OPC_HaltMomentum, 4},
	{OPC_ClearInertia, 4},
	{OPC_SetActionFlags, 8},
	{OPC_CreateParticle, 96},
	{OPC_AddBattleActor, 80},
	{OPC_AddAirJump, 8},
	{OPC_AddAirDash, 8},
	{OPC_SetAirDashTimer, 8},
	{OPC_CheckInput, 4},
	{OPC_CheckInputRaw, 8},
	{OPC_MakeInput, 4},
	{OPC_MakeInputSequenceBitmask, 8},
	{OPC_MakeInputLenience, 8},
	{OPC_MakeInputImpreciseCount, 8},
	{OPC_MakeInputAllowDisable, 8},
	{OPC_MakeInputMethod, 8},
	{OPC_BeginStateDefine, 68},
	{OPC_EndStateDefine, 4},
	{OPC_SetStateType, 8},
	{OPC_SetEntryState, 8},
	{OPC_AddInputCondition, 4},
	{OPC_AddInputConditionList, 4},
	{OPC_AddStateCondition, 8},
	{OPC_IsFollowupMove, 8},
	{OPC_SetStateObjectID, 8},
	{OPC_SetParentState, 68},
	{OPC_EnableState, 8},
	{OPC_DisableState, 8},
	{OPC_EnableAll, 4},
	{OPC_DisableAll, 4},
	{OPC_EnableFlip, 8},
	{OPC_ForceEnableFarNormal, 8},
};

USTRUCT()
struct FScriptBlockOffsets
{
	GENERATED_BODY()
	
	uint32 OnEnterOffset = -1;
	uint32 OnUpdateOffset = -1;
	uint32 OnExitOffset = -1;
	uint32 OnLandingOffset = -1;
	uint32 OnHitOffset = -1;
	uint32 OnBlockOffset = -1;
	uint32 OnHitOrBlockOffset = -1;
	uint32 OnCounterHitOffset = -1;
	uint32 OnSuperFreezeOffset = -1;
	uint32 OnSuperFreezeEndOffset = -1;
};

USTRUCT()
struct FStateAddress
{
	GENERATED_BODY()
	
    CString<64> Name;
    uint32 OffsetAddress = 0;
};

USTRUCT()
struct FScriptAnalyzer
{
	GENERATED_BODY()
private:
	char* DataAddress = nullptr;
	char* ScriptAddress = nullptr;
	int32 StateCount = 0;
	FStateAddress* StateAddresses = nullptr;
	int32 SubroutineCount = 0;
	FStateAddress* SubroutineAddresses = nullptr;

public:
	void Initialize(char* Addr, uint32 Size, TArray<UState*>* States, TArray<USubroutine*>* Subroutines);
	void InitStateOffsets(char* Addr, uint32 Size, UNightSkyScriptState* State);
	void Analyze(char* Addr, ABattleActor* Actor);
	bool FindNextCel(char** Addr, int32 AnimTime);
	void FindMatchingEnd(char** Addr, EOpCodes EndCode);
	void FindElse(char** Addr);
	void GetAllLabels(char* Addr, TArray<FStateAddress>* Labels);
	void CheckOperation(EScriptOperation Op, int32 Operand1, int32 Operand2, int32* Return);

};

UCLASS()
class UNightSkyScriptState : public UState
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UNightSkyScriptState* ParentState;
	uint32 OffsetAddress;
	uint32 Size;
	FScriptBlockOffsets Offsets;
	bool CommonState;
};

UCLASS()
class UNightSkyScriptSubroutine : public USubroutine
{
	GENERATED_BODY()

public:
	uint32 OffsetAddress;
	bool CommonSubroutine = false;
};
