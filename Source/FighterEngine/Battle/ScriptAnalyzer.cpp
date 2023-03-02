// Fill out your copyright notice in the Description page of Project Settings.


#include "ScriptAnalyzer.h"

#include "Actors\BattleActor.h"
#include "Actors\PlayerCharacter.h"

void FScriptAnalyzer::Initialize(char* Addr, uint32 Size, TArray<UState*>* States, TArray<USubroutine*>* Subroutines)
{
	DataAddress = Addr;
	StateCount = *reinterpret_cast<int*>(Addr);
	SubroutineCount = *reinterpret_cast<int*>(Addr + 4);
	StateAddresses = reinterpret_cast<FStateAddress *>(Addr + 8);
	SubroutineAddresses = &StateAddresses[StateCount];
	ScriptAddress = reinterpret_cast<char*>(&SubroutineAddresses[SubroutineCount]);

	for (int i = 0; i < StateCount; i++)
	{
		UNightSkyScriptState *NewState = NewObject<UNightSkyScriptState>();
		NewState->Name = StateAddresses[i].Name.GetString();
		NewState->OffsetAddress = StateAddresses[i].OffsetAddress;
		uint32 StateSize;
		if (i == StateCount - 1)
		{
			StateSize = Size - NewState->OffsetAddress;
		}
		else
		{
			StateSize = StateAddresses[i + 1].OffsetAddress - NewState->OffsetAddress;
		}
		InitStateOffsets(reinterpret_cast<char *>(NewState->OffsetAddress) + reinterpret_cast<uint64_t>(ScriptAddress), StateSize, NewState);
		States->Add(NewState);
	}
	for (int i = 0; i < SubroutineCount; i++)
	{
		UNightSkyScriptSubroutine *NewSubroutine = NewObject<UNightSkyScriptSubroutine>();
		NewSubroutine->Name = SubroutineAddresses[i].Name.GetString();
		NewSubroutine->OffsetAddress = SubroutineAddresses[i].OffsetAddress;
		Subroutines->Add(NewSubroutine);
	}
}

void FScriptAnalyzer::InitStateOffsets(char* Addr, uint32 Size, UNightSkyScriptState* State)
{
	while (true)
	{
		EOpCodes code = *reinterpret_cast<EOpCodes*>(Addr);
		if (code == OPC_OnEnter)
			State->Offsets.OnEnterOffset = Addr - ScriptAddress;
		else if (code == OPC_OnUpdate)
			State->Offsets.OnUpdateOffset = Addr - ScriptAddress;
		else if (code == OPC_OnExit)
			State->Offsets.OnExitOffset = Addr - ScriptAddress;
		else if (code == OPC_OnLanding)
			State->Offsets.OnLandingOffset = Addr - ScriptAddress;\
		else if (code == OPC_OnHit)
			State->Offsets.OnHitOffset = Addr - ScriptAddress;
		else if (code == OPC_OnBlock)
			State->Offsets.OnBlockOffset = Addr - ScriptAddress;
		else if (code == OPC_OnHitOrBlock)
			State->Offsets.OnHitOrBlockOffset = Addr - ScriptAddress;
		else if (code == OPC_OnCounterHit)
			State->Offsets.OnCounterHitOffset = Addr - ScriptAddress;
		else if (code == OPC_OnSuperFreeze)
			State->Offsets.OnSuperFreezeOffset = Addr - ScriptAddress;
		else if (code == OPC_OnSuperFreezeEnd)
			State->Offsets.OnSuperFreezeEndOffset = Addr - ScriptAddress;
		else if (code == OPC_EndState)
			return;
		Addr += OpCodeSizes[code];
	}
}

void FScriptAnalyzer::Analyze(char* Addr, ABattleActor* Actor)
{
	Addr += reinterpret_cast<uint64_t>(ScriptAddress);
    bool CelExecuted = false;
    TArray<FStateAddress> Labels;
    GetAllLabels(Addr, &Labels);
    UState *StateToModify = nullptr;
    char* ElseAddr = nullptr;
    while (true)
    {
        EOpCodes code = *reinterpret_cast<EOpCodes*>(Addr);
        switch (code)
        {
        case OPC_SetCel:
            {
                if (CelExecuted)
                    return;
                int32 AnimTime = *reinterpret_cast<int32 *>(Addr + 68);
                if (Actor->AnimTime == AnimTime)
                {
                    Actor->SetCelName(Addr + 4);
                    CelExecuted = true;
                }
                else if (Actor->AnimTime > AnimTime)
                {
                    while (Actor->AnimTime > AnimTime)
                    {
                        char* BakAddr = Addr;
                        Addr += OpCodeSizes[code];
                        if (FindNextCel(&Addr, Actor->AnimTime))
                        {
                            AnimTime = *reinterpret_cast<int32 *>(Addr + 68);
                            if (Actor->AnimTime == AnimTime)
                            {
                                Actor->SetCelName(Addr + 4);
                                CelExecuted = true;
                                break;
                            }
                            continue;
                        }
                        Addr = BakAddr;
                        break;
                    }
                    break;
                }
                else
                {
                    return;
                }
                break;
            }
        case OPC_CallSubroutine:
        {
            Actor->Player->CallSubroutine(Addr + 4);
            break;
        }
        case OPC_ExitState:
        {
            if (Actor->IsPlayer)
            {
                switch (Actor->Player->ActionFlags)
                {
                case ACT_Standing:
                    Actor->Player->JumpToState("Stand");
                    return;
                case ACT_Crouching:
                    Actor->Player->JumpToState("Crouch");
                    return;
                case ACT_Jumping:
                    Actor->Player->JumpToState("VJump");
                    return;
                default:
                    return;
                }
            }
        }
        case OPC_EndBlock:
        {
            return;
        }
        case OPC_GotoLabel:
        {
            CString<64> LabelName;
            LabelName.SetString(Addr + 4);
            for (FStateAddress Label : Labels)
            {
                if (!strcmp(Label.Name.GetString(), LabelName.GetString()))
                {
                    Addr = ScriptAddress + Label.OffsetAddress;
                    char* CelAddr = Addr;
                    if (FindNextCel(&CelAddr, Actor->AnimTime))
                    {
                        Addr = CelAddr;
                        Actor->AnimTime = *reinterpret_cast<int32 *>(Addr + 68);
                        Actor->SetCelName(Addr + 4);
                        code = OPC_SetCel;
                    }
                    break;
                }
            }
            break;
        }
        case OPC_EndLabel:
        {
            int32 AnimTime = *reinterpret_cast<int32 *>(Addr + 4);
            if (Actor->AnimTime < AnimTime)
                return;
            break;
        }
        case OPC_BeginStateDefine:
        {
            CString<64> StateName;
            StateName.SetString(Addr + 4);
            int32 Index = Actor->Player->StateMachine.GetStateIndex(StateName.GetString());
            if (Index != INDEX_NONE)
                StateToModify = Actor->Player->StateMachine.States[Index];
            break;
        }
        case OPC_EndStateDefine:
            StateToModify = nullptr;
            break;
        case OPC_SetStateType:
            if (StateToModify)
            {
                StateToModify->StateType = *reinterpret_cast<EStateType*>(Addr + 4);
            }
            break;
        case OPC_SetEntryState:
            if (StateToModify)
            {
                StateToModify->EntryState = *reinterpret_cast<EEntryState*>(Addr + 4);
            }
            break;
        case OPC_AddInputCondition:
            if (StateToModify)
            {
                if (StateToModify->InputConditionList.Num() == 0)
                    StateToModify->InputConditionList.Add(FInputConditionList());
                FInputCondition Condition;
                for (int i = 0; i < 32; i++)
                {
                    if (Actor->Player->SavedInputCondition.Sequence[i].InputFlag != InputNone)
                    {
                        Condition.Sequence.Add(Actor->Player->SavedInputCondition.Sequence[i]);
                        continue;
                    }
                    break;
                }
                Condition.Lenience = Actor->Player->SavedInputCondition.Lenience;
                Condition.ImpreciseInputCount = Actor->Player->SavedInputCondition.ImpreciseInputCount;
                Condition.bInputAllowDisable = Actor->Player->SavedInputCondition.bInputAllowDisable;
                Condition.Method = Actor->Player->SavedInputCondition.Method;
                StateToModify->InputConditionList[StateToModify->InputConditionList.Num() - 1].InputConditions.Add(Condition);
                break;
            }
        case OPC_AddInputConditionList:
            if (StateToModify)
            {
                StateToModify->InputConditionList.Add(FInputConditionList());
            }
            break;
        case OPC_AddStateCondition:
            if (StateToModify)
            {
                StateToModify->StateConditions.Add(*reinterpret_cast<EStateCondition*>(Addr + 4));
            }
            break;
        case OPC_IsFollowupMove:
            if (StateToModify)
            {
                StateToModify->IsFollowupState = *reinterpret_cast<bool *>(Addr + 4);
            }
            break;
        case OPC_SetStateObjectID:
            if (StateToModify)
            {
                StateToModify->ObjectID = *reinterpret_cast<int32 *>(Addr + 4);
            }
            break;
        case OPC_BeginState:
            break;
        case OPC_EndState:
            return;
        case OPC_BeginSubroutine:
            break;
        case OPC_EndSubroutine:
            return;
        case OPC_CallSubroutineWithArgs:
            break;
        case OPC_OnEnter:
            break;
        case OPC_OnUpdate:
            break;
        case OPC_OnExit:
            break;
        case OPC_OnLanding:
            break;
        case OPC_OnHit:
            break;
        case OPC_OnBlock:
            break;
        case OPC_OnHitOrBlock:
            break;
        case OPC_OnCounterHit:
            break;
        case OPC_OnSuperFreeze:
            break;
        case OPC_OnSuperFreezeEnd:
            break;
        case OPC_BeginLabel:
            break;
        case OPC_If:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            if (Operand != 0)
            {
                break;
            }

            FindMatchingEnd(&Addr, OPC_EndIf);
            ElseAddr = Addr;
            FindElse(&ElseAddr);
            code = OPC_EndIf;
            break;
        }
        case OPC_IsOnFrame:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                if (Actor->IsOnFrame(Operand))
                    Actor->StoredRegister = 1;
                else
                    Actor->StoredRegister = 0;
                break;
            }
        case OPC_EndIf:
            break;
        case OPC_IfOperation:
        {
            int32 Operand1 = *reinterpret_cast<int32 *>(Addr + 12);
            if (*reinterpret_cast<int32 *>(Addr + 8) > 0)
            {
                Operand1 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand1));
            }
            int32 Operand2 = *reinterpret_cast<int32 *>(Addr + 20);
            if (*reinterpret_cast<int32 *>(Addr + 16) > 0)
            {
                Operand2 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand2));
            }
            EScriptOperation Op = *reinterpret_cast<EScriptOperation *>(Addr + 4);
            CheckOperation(Op, Operand1, Operand2, &Actor->StoredRegister);
            if (Actor->StoredRegister != 0)
            {
                break;
            }

            FindMatchingEnd(&Addr, OPC_EndIf);
            ElseAddr = Addr;
            FindElse(&ElseAddr);
            code = OPC_EndIf;
            break;
        }
        case OPC_IfNot:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            if (Operand == 0)
            {
                break;
            }
                
            FindMatchingEnd(&Addr, OPC_EndIf);
            ElseAddr = Addr;
            FindElse(&ElseAddr);
            code = OPC_EndIf;
            break;
        };
        case OPC_Else:
            if (ElseAddr == Addr)
            {
                ElseAddr = nullptr;
                break;
            }
            else
            {
                FindMatchingEnd(&Addr, OPC_EndElse);
                code = OPC_EndElse;
                break;
            }
        case OPC_EndElse:
            break;
        case OPC_GotoLabelIf:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8 + 64);
                if (*reinterpret_cast<int32 *>(Addr + 4 + 64) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                if (Operand != 0)
                {
                    CString<64> LabelName;
                    LabelName.SetString(Addr + 4);
                    for (FStateAddress Label : Labels)
                    {
                        if (!strcmp(Label.Name.GetString(), LabelName.GetString()))
                        {
                            Addr = ScriptAddress + Label.OffsetAddress;
                            char* CelAddr = Addr;
                            if (FindNextCel(&CelAddr, Actor->AnimTime))
                            {
                                Addr = CelAddr;
                                Actor->AnimTime = *reinterpret_cast<int32 *>(Addr + 68) - 1;
                                Actor->SetCelName(Addr + 4);
                                code = OPC_SetCel;
                            }
                            break;
                        }
                    }
                    break;
                }
                break;
            }
        case OPC_GotoLabelIfOperation:
            {
                int32 Operand1 = *reinterpret_cast<int32 *>(Addr + 12 + 64);
                if (*reinterpret_cast<int32 *>(Addr + 8 + 64) > 0)
                {
                    Operand1 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand1));
                }
                int32 Operand2 = *reinterpret_cast<int32 *>(Addr + 20 + 64);
                if (*reinterpret_cast<int32 *>(Addr + 16 + 64) > 0)
                {
                    Operand2 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand2));
                }
                EScriptOperation Op = *reinterpret_cast<EScriptOperation *>(Addr + 4 + 64);
                CheckOperation(Op, Operand1, Operand2, &Actor->StoredRegister);
                if (Actor->StoredRegister != 0)
                {
                    CString<64> LabelName;
                    LabelName.SetString(Addr + 4);
                    for (FStateAddress Label : Labels)
                    {
                        if (!strcmp(Label.Name.GetString(), LabelName.GetString()))
                        {
                            Addr = ScriptAddress + Label.OffsetAddress;
                            char* CelAddr = Addr;
                            if (FindNextCel(&CelAddr, Actor->AnimTime))
                            {
                                Addr = CelAddr;
                                Actor->AnimTime = *reinterpret_cast<int32 *>(Addr + 68) - 1;
                                Actor->SetCelName(Addr + 4);
                                code = OPC_SetCel;
                            }
                            break;
                        }
                    }
                    break;
                }
                break;
            }
        case OPC_GotoLabelIfNot:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8 + 64);
                if (*reinterpret_cast<int32 *>(Addr + 4 + 64) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                if (Operand == 0)
                {
                    CString<64> LabelName;
                    LabelName.SetString(Addr + 4);
                    for (FStateAddress Label : Labels)
                    {
                        if (!strcmp(Label.Name.GetString(), LabelName.GetString()))
                        {
                            Addr = ScriptAddress + Label.OffsetAddress;
                            char* CelAddr = Addr;
                            if (FindNextCel(&CelAddr, Actor->AnimTime))
                            {
                                Addr = CelAddr;
                                Actor->AnimTime = *reinterpret_cast<int32 *>(Addr + 68) - 1;
                                Actor->SetCelName(Addr + 4);
                                code = OPC_SetCel;
                            }
                            break;
                        }
                    }
                    break;
                }
                break;
            }
        case OPC_GetPlayerStats:
        {
            EPlayerStats Stat = *reinterpret_cast<EPlayerStats*>(Addr + 4);
            int32 Val = 0;
            switch(Stat)
            {
            case PLY_FWalkSpeed:
                Val = Actor->Player->FWalkSpeed;
                break;
            case PLY_BWalkSpeed:
                Val = -Actor->Player->BWalkSpeed;
                break;
            case PLY_FDashInitSpeed:
                Val = Actor->Player->FDashInitSpeed;
                break;
            case PLY_FDashAccel:
                Val = Actor->Player->FDashAccel;
                break;
            case PLY_FDashMaxSpeed:
                Val = Actor->Player->FDashMaxSpeed;
                break;
            case PLY_FDashFriction:
                Val = Actor->Player->FDashFriction;
                break;
            case PLY_BDashSpeed:
                Val = -Actor->Player->BDashSpeed;
                break;
            case PLY_BDashHeight:
                Val = Actor->Player->BDashHeight;
                break;
            case PLY_BDashGravity:
                Val = Actor->Player->BDashGravity;
                break;
            case PLY_JumpHeight:
                Val = Actor->Player->JumpHeight;
                break;
            case PLY_FJumpSpeed:
                Val = Actor->Player->FJumpSpeed;
                break;
            case PLY_BJumpSpeed:
                Val = -Actor->Player->BJumpSpeed;
                break;
            case PLY_JumpGravity:
                Val = Actor->Player->JumpGravity;
                break;
            case PLY_SuperJumpHeight:
                Val = Actor->Player->SuperJumpHeight;
                break;
            case PLY_FSuperJumpSpeed:
                Val = Actor->Player->FSuperJumpSpeed;
                break;
            case PLY_BSuperJumpSpeed:
                Val = -Actor->Player->BSuperJumpSpeed;
                break;
            case PLY_SuperJumpGravity:
                Val = Actor->Player->SuperJumpGravity;
                break;
            case PLY_FAirDashSpeed:
                Val = Actor->Player->FAirDashSpeed;
                break;
            case PLY_BAirDashSpeed:
                Val = -Actor->Player->BAirDashSpeed;
                break;
            }
            Actor->StoredRegister = Val;
            break;
        }
        case OPC_SetPosX:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            Actor->SetPosX(Operand);
            break;
        }
        case OPC_AddPosX:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            Actor->AddPosX(Operand);
            break;
        }
        case OPC_AddPosXRaw:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            Actor->AddPosXRaw(Operand);
            break;
        }
        case OPC_SetPosY:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            Actor->SetPosY(Operand);
            break;
        }
        case OPC_AddPosY:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            Actor->AddPosY(Operand);
            break;
        }
        case OPC_SetSpeedX:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            Actor->SetSpeedX(Operand);
            break;
        }
        case OPC_AddSpeedX:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            Actor->AddSpeedX(Operand);
            break;
        }
        case OPC_SetSpeedY:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            Actor->SetSpeedY(Operand);
            break;
        }
        case OPC_AddSpeedY:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            Actor->AddSpeedY(Operand);
            break;
        }
        case OPC_SetSpeedXPercent:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->SetSpeedXPercent(Operand);
                break;
            }
        case OPC_SetSpeedXPercentPerFrame:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->SetSpeedXPercentPerFrame(Operand);
                break;
            }
        case OPC_SetSpeedYPercent:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->SetSpeedYPercent(Operand);
                break;
            }
        case OPC_SetSpeedYPercentPerFrame:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->SetSpeedYPercentPerFrame(Operand);
                break;
            }
        case OPC_SetGravity:
        {
            int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
            if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
            {
                Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
            }
            Actor->SetGravity(Operand);
            break;
        }
        case OPC_EnableState:
        {
            if (Actor->IsPlayer)
            {
                Actor->Player->EnableState(*reinterpret_cast<EEnableFlags *>(Addr + 4));
            }
            break;
        }
        case OPC_DisableState:
        {
            if (Actor->IsPlayer)
            {
                Actor->Player->DisableState(*reinterpret_cast<EEnableFlags *>(Addr + 4));
            }
            break;
        }
        case OPC_EnableAll:
        {
            if (Actor->IsPlayer)
            {
                Actor->Player->EnableAll();
            }
            break;
        }
        case OPC_DisableAll:
        {
            if (Actor->IsPlayer)
            {
                Actor->Player->DisableAll();
            }
            break;
        }
        case OPC_EnableFlip:
            Actor->EnableFlip(*reinterpret_cast<bool *>(Addr + 4));
            break;
        case OPC_ForceEnableFarNormal:
        {
            if (Actor->IsPlayer)
            {
                Actor->Player->ForceEnableFarNormal(*reinterpret_cast<bool *>(Addr + 4));
            }
            break;
        }
        case OPC_HaltMomentum: 
            Actor->HaltMomentum();
            break;
        case OPC_ClearInertia:
            Actor->ClearInertia();
            break;
        case OPC_SetActionFlags:
            if (Actor->IsPlayer)
            {
                Actor->Player->SetActionFlags(*reinterpret_cast<EActionFlags*>(Addr + 4));
            }
            break;
        case OPC_SetDefaultLandingAction:
            if (Actor->IsPlayer)
            {
                Actor->Player->SetDefaultLandingAction(*reinterpret_cast<bool*>(Addr + 4));
            }
            break;
        case OPC_CheckInput:
            {
                FInputCondition Condition;
                for (int i = 0; i < 32; i++)
                {
                    if (Actor->Player->SavedInputCondition.Sequence[i].InputFlag != InputNone)
                    {
                        Condition.Sequence.Add(Actor->Player->SavedInputCondition.Sequence[i]);
                    }
                }
                Condition.Lenience = Actor->Player->SavedInputCondition.Lenience;
                Condition.ImpreciseInputCount = Actor->Player->SavedInputCondition.ImpreciseInputCount;
                Condition.bInputAllowDisable = Actor->Player->SavedInputCondition.bInputAllowDisable;
                Condition.Method = Actor->Player->SavedInputCondition.Method;
                Actor->StoredRegister = Actor->Player->CheckInput(Condition);
                break;
            }
        case OPC_CheckInputRaw: 
            Actor->StoredRegister = Actor->Player->CheckInputRaw(*reinterpret_cast<EInputFlags*>(Addr + 4));
            break;
        case OPC_JumpToState:
            if (Actor->IsPlayer)
            {
                Actor->Player->JumpToState(Addr + 4);
                return;
            }
            break;
        case OPC_SetParentState:
            {
                if (StateToModify)
                {
                    for (auto State : Actor->Player->CommonStates)
                    {
                        CString<64> TmpString;
                        TmpString.SetString(Addr + 4);
                        if (State->Name == TmpString.GetString())
                        {
                            reinterpret_cast<UNightSkyScriptState*>(StateToModify)->ParentState = reinterpret_cast<UNightSkyScriptState*>(State);
                            break;
                        }
                    }
                }
                break;
            }
        case OPC_AddAirJump:
            if (Actor->IsPlayer)
            {
                Actor->Player->AddAirJump(*reinterpret_cast<int32*>(Addr + 4));
            }
            break;
        case OPC_AddAirDash: 
            if (Actor->IsPlayer)
            {
                Actor->Player->AddAirDash(*reinterpret_cast<int32*>(Addr + 4));
            }
            break;
        case OPC_AddGravity: 
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->AddGravity(Operand);
                break;
            }
        case OPC_SetInertia:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->SetInertia(Operand);
                break;
            }
        case OPC_AddInertia:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->AddInertia(Operand);
                break;
            }
        case OPC_EnableInertia:
            {
                if (*reinterpret_cast<int32 *>(Addr + 4) != 0)
                    Actor->EnableInertia();
                else
                    Actor->DisableInertia();
                break;
            }
        case OPC_ModifyInternalValue:
            {
                int32 Operand1 = *reinterpret_cast<int32 *>(Addr + 12);
                bool IsOperand1InternalVal = false;
                EInternalValue Val1;
                if (*reinterpret_cast<int32 *>(Addr + 8) > 0)
                {
                    Val1 = static_cast<EInternalValue>(Operand1);
                    Operand1 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand1));
                    IsOperand1InternalVal = true;
                }
                int32 Operand2 = *reinterpret_cast<int32 *>(Addr + 20);
                if (*reinterpret_cast<int32 *>(Addr + 16) > 0)
                {
                    Operand2 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand2));
                }
                EScriptOperation Op = *reinterpret_cast<EScriptOperation *>(Addr + 4);
                int32 Temp;
                CheckOperation(Op, Operand1, Operand2, &Temp);
                if (IsOperand1InternalVal)
                {
                    Actor->SetInternalValue(Val1, Temp);
                }
                break;
            }
        case OPC_StoreInternalValue:
            {
                int32 Operand1 = *reinterpret_cast<int32 *>(Addr + 8);
                bool IsOperand1InternalVal = false;
                EInternalValue Val1;
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Val1 = static_cast<EInternalValue>(Operand1);
                    Operand1 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand1));
                    IsOperand1InternalVal = true;
                }
                int32 Operand2 = *reinterpret_cast<int32 *>(Addr + 16);
                if (*reinterpret_cast<int32 *>(Addr + 12) > 0)
                {
                    Operand2 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand2));
                }
                if (IsOperand1InternalVal)
                {
                    Actor->SetInternalValue(Val1, Operand2);
                }
                break;
            }
        case OPC_ModifyInternalValueAndSave:
            {
                int32 Operand1 = *reinterpret_cast<int32 *>(Addr + 12);
                if (*reinterpret_cast<int32 *>(Addr + 8) > 0)
                {
                    Operand1 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand1));
                }
                int32 Operand2 = *reinterpret_cast<int32 *>(Addr + 20);
                if (*reinterpret_cast<int32 *>(Addr + 16) > 0)
                {
                    Operand2 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand2));
                }
                EScriptOperation Op = *reinterpret_cast<EScriptOperation *>(Addr + 4);
                int32 Temp;
                CheckOperation(Op, Operand1, Operand2, &Temp);
                int32 Operand3 = *reinterpret_cast<int32 *>(Addr + 28);
                bool IsOperand3InternalVal = false;
                EInternalValue Val3;
                if (*reinterpret_cast<int32 *>(Addr + 24) > 0)
                {
                    Val3 = static_cast<EInternalValue>(Operand3);
                    Operand3 = Actor->GetInternalValue(static_cast<EInternalValue>(Operand3));
                    IsOperand3InternalVal = true;
                }
                if (IsOperand3InternalVal)
                {
                    Actor->SetInternalValue(Val3, Temp);
                }
                break;
            }
        case OPC_SetAirDashTimer: 
            if (Actor->IsPlayer)
            {
                Actor->Player->SetAirDashTimer(*reinterpret_cast<bool *>(Addr + 4));
            }
            break;
        case OPC_SetAirDashNoAttackTimer: 
            if (Actor->IsPlayer)
            {
                Actor->Player->SetAirDashNoAttackTimer(*reinterpret_cast<bool *>(Addr + 4));
            }
            break;
        case OPC_MakeInput:
            {
                if (Actor->IsPlayer)
                {
                    Actor->Player->SavedInputCondition = FSavedInputCondition();
                }
                break;
            }
        case OPC_MakeInputSequenceBitmask:
            {
                if (Actor->IsPlayer)
                {
                    for (int i = 0; i < 32; i++)
                    {
                        if (Actor->Player->SavedInputCondition.Sequence[i].InputFlag == InputNone)
                        {
                            Actor->Player->SavedInputCondition.Sequence[i].InputFlag = *reinterpret_cast<EInputFlags*>(Addr + 4);
                            break;
                        }
                    }
                }
                break;
            }
        case OPC_MakeInputLenience:
            {
                if (Actor->IsPlayer)
                {
                    Actor->Player->SavedInputCondition.Lenience = *reinterpret_cast<int*>(Addr + 4);
                }
                break;
            }
        case OPC_MakeInputImpreciseCount:
            {
                if (Actor->IsPlayer)
                {
                    Actor->Player->SavedInputCondition.ImpreciseInputCount = *reinterpret_cast<int*>(Addr + 4);
                }
                break;
            }
        case OPC_MakeInputAllowDisable:
            {
                if (Actor->IsPlayer)
                {
                    Actor->Player->SavedInputCondition.bInputAllowDisable = *reinterpret_cast<bool*>(Addr + 4);
                }
                break;
            }
        case OPC_MakeInputMethod:
            {
                if (Actor->IsPlayer)
                {
                    Actor->Player->SavedInputCondition.Method = *reinterpret_cast<EInputMethod*>(Addr + 4);
                }
                break;
            }
        case OPC_CreateParticle:
            {
                char* ParticleName = Addr + 4;
                if (strncmp(ParticleName, "cmn", 3) == 0)
                {
                    Actor->CreateCommonParticle(ParticleName, *reinterpret_cast<EPosType*>(Addr + 68), 
                        FVector(*reinterpret_cast<float*>(Addr + 72), *reinterpret_cast<float*>(Addr + 76), *reinterpret_cast<float*>(Addr + 80)),
                        FRotator(*reinterpret_cast<float*>(Addr + 84), *reinterpret_cast<float*>(Addr + 88), *reinterpret_cast<float*>(Addr + 92)));
                }
                else
                {
                    Actor->CreateCharaParticle(ParticleName, *reinterpret_cast<EPosType*>(Addr + 68), 
                        FVector(*reinterpret_cast<float*>(Addr + 72), *reinterpret_cast<float*>(Addr + 76), *reinterpret_cast<float*>(Addr + 80)),
                        FRotator(*reinterpret_cast<float*>(Addr + 84), *reinterpret_cast<float*>(Addr + 88), *reinterpret_cast<float*>(Addr + 92)));
                }
            }
        case OPC_AddBattleActor:
            {
                char* ParticleName = Addr + 4;
                if (strncmp(ParticleName, "cmn", 3) == 0)
                {
                    Actor->Player->AddCommonBattleActor(ParticleName, *reinterpret_cast<int32*>(Addr + 68),
                        *reinterpret_cast<int32*>(Addr + 72), *reinterpret_cast<EPosType*>(Addr + 76));                }
                else
                {
                    Actor->Player->AddBattleActor(ParticleName, *reinterpret_cast<int32*>(Addr + 68),
                        *reinterpret_cast<int32*>(Addr + 72), *reinterpret_cast<EPosType*>(Addr + 76));
                }
            }
        case OPC_EnableHit:
            Actor->EnableHit(*reinterpret_cast<bool*>(Addr + 4));
            break;
        case OPC_SetAttacking:
            Actor->SetAttacking(*reinterpret_cast<bool*>(Addr + 4));
            break;
        case OPC_EnableSpecialCancel:
            if (Actor->IsPlayer)
                Actor->Player->EnableSpecialCancel(*reinterpret_cast<bool*>(Addr + 4));
            break;
        case OPC_EnableSuperCancel:
            if (Actor->IsPlayer)
                Actor->Player->EnableSuperCancel(*reinterpret_cast<bool*>(Addr + 4));
            break;
        case OPC_EnableChainCancel:
            if (Actor->IsPlayer)
                Actor->Player->EnableChainCancel(*reinterpret_cast<bool*>(Addr + 4));
            break;
        case OPC_EnableWhiffCancel:
            if (Actor->IsPlayer)
                Actor->Player->EnableWhiffCancel(*reinterpret_cast<bool*>(Addr + 4));
            break;
        case OPC_EnableCancelIntoSelf:
            if (Actor->IsPlayer)
                Actor->Player->EnableCancelIntoSelf(*reinterpret_cast<bool*>(Addr + 4));
            break;
        case OPC_SetAttackLevel:
            Actor->HitEffect.AttackLevel = *reinterpret_cast<int32*>(Addr + 4);
            Actor->CounterHitEffect.AttackLevel = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetCounterAttackLevel:
            Actor->CounterHitEffect.AttackLevel = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetHitstun:
            Actor->HitEffect.Hitstun = *reinterpret_cast<int32*>(Addr + 4);
            Actor->CounterHitEffect.Hitstun = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetCounterHitstun:
            Actor->CounterHitEffect.Hitstun = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetUntech:
            Actor->HitEffect.Untech = *reinterpret_cast<int32*>(Addr + 4);
            Actor->CounterHitEffect.Untech = *reinterpret_cast<int32*>(Addr + 4) * 2;
            break;
        case OPC_SetCounterUntech:
            Actor->CounterHitEffect.Untech = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetDamage:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->HitEffect.HitDamage = Operand;
                Actor->CounterHitEffect.HitDamage = Operand * 11 / 10;
            }
            break;
        case OPC_SetCounterDamage:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->CounterHitEffect.HitDamage = Operand;
            }
            break;
        case OPC_SetMinimumDamagePercent:
            Actor->HitEffect.MinimumDamagePercent = *reinterpret_cast<int32*>(Addr + 4);
            Actor->CounterHitEffect.MinimumDamagePercent = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetCounterMinimumDamagePercent:
            Actor->CounterHitEffect.MinimumDamagePercent = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetInitialProration:
            Actor->HitEffect.InitialProration = *reinterpret_cast<int32*>(Addr + 4);
            Actor->CounterHitEffect.InitialProration = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetCounterInitialProration:
            Actor->CounterHitEffect.InitialProration = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetForcedProration:
            Actor->HitEffect.ForcedProration = *reinterpret_cast<int32*>(Addr + 4);
            Actor->CounterHitEffect.ForcedProration = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetCounterForcedProration:
            Actor->CounterHitEffect.ForcedProration = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetHitPushbackX:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->HitEffect.HitPushbackX = Operand;
                Actor->CounterHitEffect.HitPushbackX = Operand;
            }
            break;
        case OPC_SetCounterHitPushbackX:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->CounterHitEffect.HitPushbackX = Operand;
            }
            break;
        case OPC_SetAirHitPushbackX:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->HitEffect.AirHitPushbackX = Operand;
                Actor->CounterHitEffect.AirHitPushbackX = Operand;
            }
            break;
        case OPC_SetCounterAirHitPushbackX:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->CounterHitEffect.AirHitPushbackX = Operand;
            }
            break;
        case OPC_SetAirHitPushbackY:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->HitEffect.AirHitPushbackY = Operand;
                Actor->CounterHitEffect.AirHitPushbackY = Operand;
            }
            break;
        case OPC_SetCounterAirHitPushbackY:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->CounterHitEffect.AirHitPushbackY = Operand;
            }
            break;
        case OPC_SetHitGravity:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->HitEffect.HitGravity = Operand;
                Actor->CounterHitEffect.HitGravity = Operand;
            }
            break;
        case OPC_SetCounterHitGravity:
            {
                int32 Operand = *reinterpret_cast<int32 *>(Addr + 8);
                if (*reinterpret_cast<int32 *>(Addr + 4) > 0)
                {
                    Operand = Actor->GetInternalValue(static_cast<EInternalValue>(Operand));
                }
                Actor->CounterHitEffect.HitGravity = Operand;
            }
            break;
        case OPC_SetGroundHitAction:
            Actor->HitEffect.GroundHitAction = *reinterpret_cast<EHitAction*>(Addr + 4);
            Actor->CounterHitEffect.GroundHitAction = *reinterpret_cast<EHitAction*>(Addr + 4);
            break;
        case OPC_SetCounterGroundHitAction:
            Actor->CounterHitEffect.GroundHitAction = *reinterpret_cast<EHitAction*>(Addr + 4);
            break;
        case OPC_SetAirHitAction:
            Actor->HitEffect.AirHitAction = *reinterpret_cast<EHitAction*>(Addr + 4);
            Actor->CounterHitEffect.AirHitAction = *reinterpret_cast<EHitAction*>(Addr + 4);
            break;
        case OPC_SetCounterAirHitAction:
            Actor->CounterHitEffect.AirHitAction = *reinterpret_cast<EHitAction*>(Addr + 4);
            break;
        case OPC_SetKnockdownTime:
            Actor->HitEffect.KnockdownTime = *reinterpret_cast<int32*>(Addr + 4);
            Actor->CounterHitEffect.KnockdownTime = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetCounterKnockdownTime:
            Actor->CounterHitEffect.KnockdownTime = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetHitstop:
            Actor->HitEffect.Hitstop = *reinterpret_cast<int32*>(Addr + 4);
            Actor->CounterHitEffect.Hitstop = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetCounterHitstop:
            Actor->CounterHitEffect.Hitstop = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetBlockstun:
            Actor->HitEffect.Blockstun = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetBlockstopModifier:
            Actor->HitEffect.BlockstopModifier = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetChipDamagePercent:
            Actor->HitEffect.ChipDamagePercent = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_SetHitAngle:
            Actor->HitEffect.HitAngle = *reinterpret_cast<int32*>(Addr + 4);
            Actor->CounterHitEffect.HitAngle = *reinterpret_cast<int32*>(Addr + 4);
            break;
        case OPC_AddChainCancelOption:
            {
                if (Actor->IsPlayer)
                {
                    CString<64> StateName;
                    StateName.SetString(Addr + 4);
                    Actor->Player->AddChainCancelOption(StateName.GetString());
                }
            }
        case OPC_AddWhiffCancelOption:
            {
                if (Actor->IsPlayer)
                {
                    CString<64> StateName;
                    StateName.SetString(Addr + 4);
                    Actor->Player->AddWhiffCancelOption(StateName.GetString());
                }
            }
        default:
            break;
        }
        Addr += OpCodeSizes[code];
    }
}

bool FScriptAnalyzer::FindNextCel(char** Addr, int32 AnimTime)
{
    
    while (true)
    {
        EOpCodes code = *reinterpret_cast<EOpCodes*>(*Addr);
        switch (code)
        {
        case OPC_SetCel:
            return true;
        case OPC_EndLabel:
            {
                if (AnimTime > *reinterpret_cast<int*>(*Addr + 4))
                {
                    break;
                }
                return false;
            }
        case OPC_ExitState:
        case OPC_EndBlock:
        case OPC_EndState:
        case OPC_EndSubroutine:
            return false;
        default:
            break;
        }
        *Addr += OpCodeSizes[code];
    }
}

void FScriptAnalyzer::FindMatchingEnd(char** Addr, EOpCodes EndCode)
{
    int32 Depth = -1;
    while (true)
    {
        EOpCodes code = *reinterpret_cast<EOpCodes*>(*Addr);
        if (EndCode == OPC_EndIf)
        {
            if (code == OPC_If || code == OPC_IfNot || code == OPC_IfOperation)
                Depth++;
        }
        else if (EndCode == OPC_EndElse)
        {
            if (code == OPC_Else)
                Depth++;
        }
        
        if (code == EndCode)
        {
            if (Depth > 0)
                Depth--;
            else
                return;
        }
        if (code == OPC_EndBlock || code == OPC_ExitState || code == OPC_EndSubroutine || code == OPC_EndLabel)
            return;
        *Addr += OpCodeSizes[code];
    }
}

void FScriptAnalyzer::FindElse(char** Addr)
{
    int32 Depth = -1;
    while (true)
    {
        EOpCodes code = *reinterpret_cast<EOpCodes *>(*Addr);

        if (code == OPC_If || code == OPC_IfNot || code == OPC_IfOperation)
            Depth++;
        if (code == OPC_Else)
        {
            if (Depth > 0)
                Depth--;
            else
                return;                
        }
        if (code == OPC_EndBlock || code == OPC_ExitState || code == OPC_EndSubroutine || code == OPC_EndLabel)
        {
            *Addr = 0;
            return;
        }
        *Addr += OpCodeSizes[code];
    }
}

void FScriptAnalyzer::GetAllLabels(char* Addr, TArray<FStateAddress>* Labels)
{
    while (true)
    {
        EOpCodes code = *reinterpret_cast<EOpCodes*>(Addr);
        if (code == OPC_BeginLabel)
        {
            CString<64> LabelName;
            LabelName.SetString(Addr + 4);
            FStateAddress Label;
            Label.Name = LabelName;
            Label.OffsetAddress = Addr - ScriptAddress;
            Labels->Add(Label);
        }
        if (code == OPC_EndBlock || code == OPC_EndSubroutine || code == OPC_EndState)
            return;
        Addr += OpCodeSizes[code];
    }
}

void FScriptAnalyzer::CheckOperation(EScriptOperation Op, int32 Operand1, int32 Operand2, int32* Return)
{
    switch (Op)
    {
    case OP_Add:
        {
            *Return = Operand1 + Operand2;
            break;
        }
    case OP_Sub:
        {
            *Return = Operand1 - Operand2;
            break;
        }
    case OP_Mul:
        {
            *Return = Operand1 * Operand2;
            break;
        }
    case OP_Div:
        {
            *Return = Operand1 / Operand2;
            break;
        }
    case OP_Mod:
        {
            *Return = Operand1 % Operand2;
            break;
        }
    case OP_And:
        {
            *Return = Operand1 && Operand2;
            break;
        }
    case OP_Or:
        {
            *Return = Operand1 || Operand2;
            break;
        }
    case OP_BitwiseAnd:
        {
            *Return = Operand1 & Operand2;
            break;
        }
    case OP_BitwiseOr:
        {
            *Return = Operand1 | Operand2;
            break;
        }
    case OP_IsEqual:
        {
            *Return = Operand1 == Operand2;
            break;
        }
    case OP_IsGreater:
        {
            *Return = Operand1 > Operand2;
            break;
        }
    case OP_IsLesser:
        {
            *Return = Operand1 < Operand2;
            break;
        }
    case OP_IsGreaterOrEqual:
        {
            *Return = Operand1 >= Operand2;
            break;
        }
    case OP_IsLesserOrEqual:
        {
            *Return = Operand1 <= Operand2;
            break;
        }
    case OP_BitDelete:
        {
            *Return = Operand1 & ~Operand2;
            break;
        }
    case OP_IsNotEqual:
        {
            *Return = Operand1 != Operand2;
            break;
        }
    }
}