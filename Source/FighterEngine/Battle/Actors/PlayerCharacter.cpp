// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "FighterGameState.h"
#include "FighterEngine/Miscellaneous/FighterGameInstance.h"

APlayerCharacter::APlayerCharacter()
{
	Player = this;
	StateMachine.Parent = this;
	ScreenCollisionActive = true;
	PushCollisionActive = true;
	FWalkSpeed = 7800;
	BWalkSpeed = 4800;
	FDashInitSpeed = 13000;
	FDashAccel = 600;
	FDashFriction = 95;
	FDashMaxSpeed = 20000;
	BDashSpeed = 14000;
	BDashHeight = 5200;
	BDashGravity = 700;
	JumpHeight = 35000;
	FJumpSpeed = 7900;
	BJumpSpeed = 5200;
	JumpGravity = 1900;
	SuperJumpHeight = 43700;
	FSuperJumpSpeed = 7900;
	BSuperJumpSpeed = 5200;
	SuperJumpGravity = 1900;
	AirDashMinimumHeight = 105000;
	FAirDashSpeed = 30000;
	BAirDashSpeed = 24500;
	FAirDashTime = 20;
	BAirDashTime = 12;
	FAirDashNoAttackTime = 5;
	BAirDashNoAttackTime = 5;
	AirJumpCount = 1;
	AirDashCount = 1;
	ActionFlags = (int)ACT_Standing;
	StandPushWidth = 110000;
	StandPushHeight = 240000;
	CrouchPushWidth = 120000;
	CrouchPushHeight = 180000;
	AirPushWidth = 100000;
	AirPushHeight = 275000;
	AirPushHeightLow = -135000;
	DefaultCommonAction = true;
	IsPlayer = true;
	IsActive = true;
	Health = 10000;
	ForwardWalkMeterGain = 12;
	ForwardJumpMeterGain = 10;
	ForwardDashMeterGain = 25;
	ForwardAirDashMeterGain = 25;
	AttackProjectileAttribute = false;
	DefaultLandingAction = true;
	for (int i = 0; i < MaxComponentCount; i++)
		ComponentVisible[i] = true;
	InitPlayer();
}

void APlayerCharacter::InitPlayer()
{
	CurrentHealth = Health;
	EnableAll();
	EnableFlip(true);
	StateName.SetString("Stand");
	if (CharaScript == nullptr)
		InitBP();
	else
		InitStates();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::InitStates()
{
	if (CommonScript == nullptr)
		return;
	uint8* CommonScriptData = CommonScript->Data.GetData();
	CommonAnalyzer.Initialize(reinterpret_cast<char*>(CommonScriptData), CommonScript->Data.Num(), &CommonStates, &CommonSubroutines);
	for (auto State : CommonStates)
	{
		State->Parent = this;
		reinterpret_cast<UNightSkyScriptState*>(State)->CommonState = true;
	}
	for (auto Subroutine : CommonSubroutines)
	{
		CommonSubroutineNames.Add(Subroutine->Name);
		Subroutine->Parent = this;
		reinterpret_cast<UNightSkyScriptSubroutine*>(Subroutine)->CommonSubroutine = true;
	}
	
	if (CommonObjScript == nullptr)
		return;
	uint8* CommonObjScriptData = CommonObjScript->Data.GetData();
	CommonObjAnalyzer.Initialize(reinterpret_cast<char*>(CommonObjScriptData), CommonObjScript->Data.Num(), &CommonObjectStates, &CommonSubroutines);
	for (auto State : CommonObjectStates)
	{
		CommonObjectStateNames.Add(State->Name);
	}

	if (CharaScript == nullptr)
		return;
	
	uint8* CharaScriptData = CharaScript->Data.GetData();
	CharaAnalyzer.Initialize(reinterpret_cast<char*>(CharaScriptData), CharaScript->Data.Num(), &StateMachine.States, &Subroutines);
	
	if (ObjScript != nullptr)
	{
		uint8* ObjScriptData = ObjScript->Data.GetData();
		ObjAnalyzer.Initialize(reinterpret_cast<char*>(ObjScriptData), ObjScript->Data.Num(), &ObjectStates, &Subroutines);
		for (auto State : ObjectStates)
		{
			ObjectStateNames.Add(State->Name);
		}
	}

	for (auto Subroutine : Subroutines)
	{
		SubroutineNames.Add(Subroutine->Name);
		Subroutine->Parent = this;
	}
	StateMachine.Parent = this;
	StateMachine.Initialize();
	StateMachine.ParentStates(CommonStates);
	CallSubroutine("CmnMatchInit");
	CallSubroutine("MatchInit");
	StateMachine.CurrentState->OnEnter();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::Update()
{
	Super::Update();
	CallSubroutine("CmnOnUpdate");
	CallSubroutine("OnUpdate");

	if (UFighterGameInstance* GameInstance = Cast<UFighterGameInstance>(GetGameInstance()); GameInstance->IsTraining)
	{
		if (!IsStunned)
			CurrentHealth = Health;
		if (PlayerIndex == 1)
			Inputs = InputL;
		if (!IsAttacking && ComboTimer <= 0)
		{
			GameState->BattleState.Meter[PlayerIndex] = GameState->BattleState.MaxMeter[PlayerIndex];
			GameState->BattleState.UniversalGauge[PlayerIndex] = GameState->BattleState.MaxUniversalGauge;
		}
	}
	if (!IsStunned)
	{
		Enemy->ComboCounter = 0;
		Enemy->ComboTimer = 0;
		TotalProration = 10000;
	}
	if (!Inputs << 27) //if no direction, set neutral input
	{
		Inputs |= (int)InputNeutral;
	}
	else
	{
		Inputs = Inputs & ~(int)InputNeutral; //remove neutral input if directional input
	}
	if (IsThrowLock)
	{
		InputBuffer.Tick(Inputs);
		HandleStateMachine(true); //handle state transitions
		if (ThrowTechTimer > 0)
		{
			FInputCondition ConditionL;
			FInputBitmask BitmaskL;
			BitmaskL.InputFlag = InputL;
			ConditionL.Sequence.Add(BitmaskL);
			ConditionL.Method = EInputMethod::Once;
			FInputCondition ConditionS;
			FInputBitmask BitmaskS;
			BitmaskS.InputFlag = InputS;
			ConditionS.Sequence.Add(BitmaskS);
			ConditionS.Method = EInputMethod::Once;
			if (CheckInput(ConditionL) && CheckInput(ConditionS) || CheckInput(ConditionS) && CheckInput(ConditionL))
			{
				ThrowTechTimer = 0;
				JumpToState("GuardBreak");
				Enemy->JumpToState("GuardBreak");
				IsThrowLock = false;
				SetInertia(-35000);
				Enemy->SetInertia(-35000);
				HitPosX = (PosX + Enemy->PosX) / 2;
				HitPosY = (PosY + Enemy->PosY) / 2 + 250000;
				CreateCommonParticle("cmn_throwtech", POS_Hit);
				return;
			}
		}
		ThrowTechTimer--;
		return;
	}
	
	if (ReceivedAttackLevel != -1)
		HandleHitAction();
	
	if (SuperFreezeTime > 0)
	{
		InputBuffer.Tick(Inputs);
		HandleStateMachine(true); //handle state transitions
		return;
	}
	if (SuperFreezeTime == 0)
	{
		BattleHudVisibility(true);
		StateMachine.CurrentState->OnSuperFreezeEnd();
		AnimTime++;
        AnimBPTime++;
	}
	
	if (Hitstop > 0)
	{
		InputBuffer.Tick(Inputs);
		HandleStateMachine(true); //handle state transitions
		return;
	}
	
	StrikeInvulnerableForTime--;
	ThrowInvulnerableForTime--;
	if (StrikeInvulnerableForTime < 0)
		StrikeInvulnerableForTime = 0;
	if (ThrowInvulnerableForTime < 0)
		ThrowInvulnerableForTime = 0;
	
	if (PosY > 0) //set jumping if above ground
	{
		SetActionFlags(ACT_Jumping);
	}
	
	HandleBufferedState();

	if (TouchingWall)
		WallTouchTimer++;
	else
		WallTouchTimer = 0;
	
	if (ComboCounter > 0)
		ComboTimer++;
		
	if (StateMachine.CurrentState->StateType == EStateType::ForwardWalk)
		AddMeter(ForwardWalkMeterGain);
	else if (StateMachine.CurrentState->StateType == EStateType::ForwardJump)
		AddMeter(ForwardJumpMeterGain);
	if (StateMachine.CurrentState->StateType == EStateType::ForwardDash)
		AddMeter(ForwardDashMeterGain);
	else if (StateMachine.CurrentState->StateType == EStateType::ForwardAirDash)
		AddMeter(ForwardAirDashMeterGain);
	MeterCooldownTimer--;
	
	if (!RoundWinInputLock)
		InputBuffer.Tick(Inputs);
	else
		InputBuffer.Tick(InputNeutral);
	
	AirDashTimer--;
	AirDashNoAttackTime--;
	if (AirDashNoAttackTime == 0)
		EnableAttacks();
	
	Hitstun--;
	if (Hitstun == 0 && !IsDead)
	{
		EnableAll();
		if (ActionFlags == ACT_Standing)
		{
			JumpToState("Stand");
		}
		else if (ActionFlags == ACT_Crouching)
		{
			JumpToState("Crouch");
		}
		TotalProration = 10000;
	}

	Untech--;
	if (Untech == 0 && !IsKnockedDown && !IsDead)
		EnableState(ENB_Tech);

	if (StateMachine.CurrentState->StateType == EStateType::Tech)
	{
		HasBeenOTG = 0;
		WallBounceEffect = FWallBounceEffect();
		GroundBounceEffect = FGroundBounceEffect();
	}
	
	if (StateMachine.CurrentState->StateType == EStateType::Hitstun && PosY <= 0 && PrevPosY > 0)
	{
		HaltMomentum();
		if (StateMachine.CurrentState->Name == "BLaunch" || StateMachine.CurrentState->Name == "Blowback")
			JumpToState("FaceUpBounce");
		else if (StateMachine.CurrentState->Name == "FLaunch")
			JumpToState("FaceDownBounce");
	}

	if (PosY <= 0 && !IsDead && GroundBounceEffect.GroundBounceCount == 0)
	{
		KnockdownTime--;
		if (Untech > 0 && PrevPosY > 0)
		{
			Untech = -1;
			IsKnockedDown = true;
			DisableState(ENB_Tech);
		}
	}

	if (StateMachine.CurrentState->StateType != EStateType::Hitstun)
	{
		KnockdownTime = -1;
		IsKnockedDown = false;
	}

	if (KnockdownTime < 0 && Blockstun < 0 && (Untech < 0 && StateMachine.CurrentState->StateType != EStateType::Hitstun) && Hitstun < 0)
		IsStunned = false;

	if (KnockdownTime == 0 && PosY <= 0 && !IsDead)
	{
		Enemy->ComboCounter = 0;
		Enemy->ComboTimer = 0;
		HasBeenOTG = 0;
		if (StateMachine.CurrentState->Name == "FaceDown" || StateMachine.CurrentState->Name == "FaceDownBounce")
			JumpToState("WakeUpFaceDown");
		else if (StateMachine.CurrentState->Name == "FaceUp" || StateMachine.CurrentState->Name == "FaceUpBounce")
			JumpToState("WakeUpFaceUp");
		TotalProration = 10000;
	}
	
	if (IsDead)
		DisableState(ENB_Tech);
	
	Blockstun--;
	if (Blockstun == 0)
	{
		if (GetCurrentStateName() == "Block")
		{
			JumpToState("Stand");
		}
		else if (GetCurrentStateName() == "CrouchBlock")
		{
			JumpToState("Crouch");
		}
		else
		{
			JumpToState("VJump");
		}
	}

	InstantBlockTimer--;
	ParryTimer--;
	CheckMissedInstantBlock();
	CheckMissedParry();
	
	HandleWallBounce();
	
	if (PosY == 0 && PrevPosY != 0) //reset air move counts on ground
	{
		CurrentAirJumpCount = AirJumpCount;
		CurrentAirDashCount = AirDashCount;
		if (DefaultLandingAction)
		{
			JumpToState("JumpLanding");
		}
		else
		{
			StateMachine.CurrentState->OnLanding();
		}
		CreateCommonParticle("cmn_jumpland_smoke", POS_Player);
		HandleGroundBounce();
	}
	HandleThrowCollision();
	CalculateUltra();
	if (GameState->BattleState.TimeUntilRoundStart <= 0)
		HandleStateMachine(false); //handle state transitions
	SetComponentVisibility();
}

void APlayerCharacter::HandleStateMachine(bool Buffer)
{
	for (int i = StateMachine.States.Num() - 1; i >= 0; i--)
	{
        if (!(CheckStateEnabled(StateMachine.States[i]->StateType) && !StateMachine.States[i]->IsFollowupState
            || FindChainCancelOption(StateMachine.States[i]->Name)
            || FindWhiffCancelOption(StateMachine.States[i]->Name)
            || CheckKaraCancel(StateMachine.States[i]->StateType) && !StateMachine.States[i]->IsFollowupState
            )) //check if the state is enabled, continue if not
        {
            continue;
        }
		if (CheckObjectPreventingState(StateMachine.States[i]->ObjectID)) //check if an object is preventing state entry, continue if so
		{
			continue;
		}
        //check current character state against entry state condition, continue if not entry state
		if (!StateMachine.CheckStateEntryCondition(StateMachine.States[i]->EntryState, ActionFlags))
        {
            continue;
        }
		if (StateMachine.States[i]->StateConditions.Num() != 0) //only check state conditions if there are any
		{
			for (int j = 0; j < StateMachine.States[i]->StateConditions.Num(); j++) //iterate over state conditions
			{
                if (!HandleStateCondition(StateMachine.States[i]->StateConditions[j])) //check state condition
                {
                    break;
                }
                if (!(j == StateMachine.States[i]->StateConditions.Num() - 1)) //have all conditions been met?
                {
                    continue;
                }
				for (FInputConditionList& List : StateMachine.States[i]->InputConditionList)
				{
					for (int v = 0; v < List.InputConditions.Num(); v++) //iterate over input conditions
					{
						//check input condition against input buffer, if not met break.
	                    if (!InputBuffer.CheckInputCondition(List.InputConditions[v]))
	                    {
	                        break;
	                    }
						if (v == List.InputConditions.Num() - 1) //have all conditions been met?
						{
							if (FindChainCancelOption(StateMachine.States[i]->Name)
								|| FindWhiffCancelOption(StateMachine.States[i]->Name)) //if cancel option, allow resetting state
							{
								if (Buffer)
								{
									BufferedStateName.SetString(StateMachine.States[i]->Name);
									return;
								}
								if (StateMachine.ForceSetState(StateMachine.States[i]->Name)) //if state set successful...
								{
									StateName.SetString(StateMachine.States[i]->Name);
									switch (StateMachine.States[i]->EntryState)
									{
									case EEntryState::Standing:
										ActionFlags = ACT_Standing;
										break;
									case EEntryState::Crouching:
										ActionFlags = ACT_Crouching;
										break;
									case EEntryState::Jumping:
										ActionFlags = ACT_Jumping;
										break;
									default:
										break;
									}
									return; //don't try to enter another state
								}
							}
							else
							{
								if (Buffer)
								{
									BufferedStateName.SetString(StateMachine.States[i]->Name);
									return;
								}
								if (StateMachine.SetState(StateMachine.States[i]->Name)) //if state set successful...
								{
									StateName.SetString(StateMachine.States[i]->Name);
									switch (StateMachine.States[i]->EntryState)
									{
									case EEntryState::Standing:
										ActionFlags = ACT_Standing;
										break;
									case EEntryState::Crouching:
										ActionFlags = ACT_Crouching;
										break;
									case EEntryState::Jumping:
										ActionFlags = ACT_Jumping;
										break;
									default:
										break;
									}
									return; //don't try to enter another state
								}
							}
						}
					}
					if (List.InputConditions.Num() == 0) //if no input condtions, set state
					{
						if (Buffer)
						{
							BufferedStateName.SetString(StateMachine.States[i]->Name);
							return;
						}
						if (StateMachine.SetState(StateMachine.States[i]->Name)) //if state set successful...
						{
							StateName.SetString(StateMachine.States[i]->Name);
							switch (StateMachine.States[i]->EntryState)
							{
							case EEntryState::Standing:
								ActionFlags = ACT_Standing;
								break;
							case EEntryState::Crouching:
								ActionFlags = ACT_Crouching;
								break;
							case EEntryState::Jumping:
								ActionFlags = ACT_Jumping;
								break;
							default:
								break;
							}
							return; //don't try to enter another state
							}
						}
					}
                continue; //this is unneeded but here for clarity.
			}
		}
		else
		{
			for (FInputConditionList& List : StateMachine.States[i]->InputConditionList)
			{
				for (int v = 0; v < List.InputConditions.Num(); v++) //iterate over input conditions
				{
					//check input condition against input buffer, if not met break.
					if (!InputBuffer.CheckInputCondition(List.InputConditions[v]))
					{
						break;
					}
					if (v == List.InputConditions.Num() - 1) //have all conditions been met?
					{
						if (FindChainCancelOption(StateMachine.States[i]->Name)
							|| FindWhiffCancelOption(StateMachine.States[i]->Name)) //if cancel option, allow resetting state
						{
							if (Buffer)
							{
								BufferedStateName.SetString(StateMachine.States[i]->Name);
								return;
							}
							if (StateMachine.ForceSetState(StateMachine.States[i]->Name)) //if state set successful...
							{
								StateName.SetString(StateMachine.States[i]->Name);
								switch (StateMachine.States[i]->EntryState)
								{
								case EEntryState::Standing:
									ActionFlags = ACT_Standing;
									break;
								case EEntryState::Crouching:
									ActionFlags = ACT_Crouching;
									break;
								case EEntryState::Jumping:
									ActionFlags = ACT_Jumping;
									break;
								default:
									break;
								}
								return; //don't try to enter another state
							}
						}
						else
						{
							if (Buffer)
							{
								BufferedStateName.SetString(StateMachine.States[i]->Name);
								return;
							}
							if (StateMachine.SetState(StateMachine.States[i]->Name)) //if state set successful...
							{
								StateName.SetString(StateMachine.States[i]->Name);
								switch (StateMachine.States[i]->EntryState)
								{
								case EEntryState::Standing:
									ActionFlags = ACT_Standing;
									break;
								case EEntryState::Crouching:
									ActionFlags = ACT_Crouching;
									break;
								case EEntryState::Jumping:
									ActionFlags = ACT_Jumping;
									break;
								default:
									break;
								}
								return; //don't try to enter another state
							}
						}
					}
				}
				if (List.InputConditions.Num() == 0) //if no input condtions, set state
				{
					if (Buffer)
					{
						BufferedStateName.SetString(StateMachine.States[i]->Name);
						return;
					}
					if (StateMachine.SetState(StateMachine.States[i]->Name)) //if state set successful...
					{
						StateName.SetString(StateMachine.States[i]->Name);
						switch (StateMachine.States[i]->EntryState)
						{
						case EEntryState::Standing:
							ActionFlags = ACT_Standing;
							break;
						case EEntryState::Crouching:
							ActionFlags = ACT_Crouching;
							break;
						case EEntryState::Jumping:
							ActionFlags = ACT_Jumping;
							break;
						default:
							break;
						}
						return; //don't try to enter another state
					}
				}
			}
		}
	}
}


void APlayerCharacter::HandleBufferedState()
{
	if (!strcmp(BufferedStateName.GetString(), ""))
	{
		if (FindChainCancelOption(BufferedStateName.GetString())
			|| FindWhiffCancelOption(BufferedStateName.GetString())) //if cancel option, allow resetting state
		{
			if (StateMachine.ForceSetState(BufferedStateName.GetString()))
			{
				StateName.SetString(BufferedStateName.GetString());
				switch (StateMachine.CurrentState->EntryState)
				{
				case EEntryState::Standing:
					ActionFlags = ACT_Standing;
					break;
				case EEntryState::Crouching:
					ActionFlags = ACT_Crouching;
					break;
				case EEntryState::Jumping:
					ActionFlags = ACT_Jumping;
					break;
				default:
					break;
				}
			}
			BufferedStateName.SetString("");
		}
		else
		{
			if (StateMachine.SetState(BufferedStateName.GetString()))
			{
				StateName.SetString(BufferedStateName.GetString());
				switch (StateMachine.CurrentState->EntryState)
				{
				case EEntryState::Standing:
					ActionFlags = ACT_Standing;
					break;
				case EEntryState::Crouching:
					ActionFlags = ACT_Crouching;
					break;
				case EEntryState::Jumping:
					ActionFlags = ACT_Jumping;
					break;
				default:
					break;
				}
			}
			BufferedStateName.SetString("");
		}
	}
}

void APlayerCharacter::SetActionFlags(EActionFlags ActionFlag)
{
	ActionFlags = (int)ActionFlag;
}

void APlayerCharacter::AddState(FString Name, UState* State)
{
	StateMachine.Parent = this;
	StateMachine.AddState(Name, State);
}

void APlayerCharacter::AddSubroutine(FString Name, USubroutine* Subroutine)
{
	Subroutine->Parent = this;
	Subroutines.Add(Subroutine);
	SubroutineNames.Add(Name);
}

void APlayerCharacter::CallSubroutine(FString Name)
{
	if (CommonSubroutineNames.Find(Name) != INDEX_NONE)
	{
		CommonSubroutines[CommonSubroutineNames.Find(Name)]->OnCall();
		return;
	}

	if (SubroutineNames.Find(Name) != INDEX_NONE)
		Subroutines[SubroutineNames.Find(Name)]->OnCall();
}

void APlayerCharacter::UseMeter(int Use)
{
	GameState->BattleState.Meter[PlayerIndex] -= Use;
}

void APlayerCharacter::UseUniversalGauge(int Use)
{
	GameState->BattleState.UniversalGauge[PlayerIndex] -= Use;
}

void APlayerCharacter::AddMeter(int Meter)
{
	if (MeterCooldownTimer > 0)
		Meter /= 10;
	GameState->BattleState.Meter[PlayerIndex] += Meter;
}

void APlayerCharacter::AddUniversalGauge(int Gauge)
{
	GameState->BattleState.UniversalGauge[PlayerIndex] += Gauge;
}

void APlayerCharacter::AddUltraFactor(int Factor)
{
	UltraFactor += Factor;
}

void APlayerCharacter::SetMeterCooldownTimer(int Timer)
{
	MeterCooldownTimer = Timer;
}

void APlayerCharacter::SetLockOpponentBurst(bool Locked)
{
	LockOpponentBurst = true;
}

void APlayerCharacter::JumpToState(FString NewName)
{
	if (StateMachine.ForceSetState(NewName))
		StateName.SetString(NewName);
	if (StateMachine.CurrentState != nullptr)
	{
		switch (StateMachine.CurrentState->EntryState)
		{
		case EEntryState::Standing:
			ActionFlags = ACT_Standing;
			break;
		case EEntryState::Crouching:
			ActionFlags = ACT_Crouching;
			break;
		case EEntryState::Jumping:
			ActionFlags = ACT_Jumping;
			break;
		default:
			break;
		}
	}
}

FString APlayerCharacter::GetCurrentStateName()
{
	return StateMachine.CurrentState->Name;
}

int32 APlayerCharacter::GetLoopCount()
{
	return LoopCounter;
}

void APlayerCharacter::IncrementLoopCount()
{
	LoopCounter += 1;
}

bool APlayerCharacter::CheckStateEnabled(EStateType StateType)
{
	switch (StateType)
	{
	case EStateType::Standing:
		if (EnableFlags & ENB_Standing)
			return true;
		break;
	case EStateType::Crouching:
		if (EnableFlags & ENB_Crouching)
			return true;
		break;
	case EStateType::NeutralJump:
	case EStateType::ForwardJump:
	case EStateType::BackwardJump:
		if (EnableFlags & ENB_Jumping || JumpCancel && HasHit && IsAttacking)
			return true;
		break;
	case EStateType::ForwardWalk:
		if (EnableFlags & ENB_ForwardWalk)
			return true;
		break;
	case EStateType::BackwardWalk:
		if (EnableFlags & ENB_BackWalk)
			return true;
		break;
	case EStateType::ForwardDash:
		if (EnableFlags & ENB_ForwardDash)
			return true;
		break;
	case EStateType::BackwardDash:
		if (EnableFlags & ENB_BackDash)
			return true;
		break;
	case EStateType::ForwardAirDash:
		if (EnableFlags & ENB_ForwardAirDash || FAirDashCancel && HasHit && IsAttacking)
			return true;
		break;
	case EStateType::BackwardAirDash:
		if (EnableFlags & ENB_BackAirDash || BAirDashCancel && HasHit && IsAttacking)
			return true;
		break;
	case EStateType::NormalAttack:
	case EStateType::NormalThrow:
		if (EnableFlags & ENB_NormalAttack)
			return true;
		break;
	case EStateType::SpecialAttack:
		if (EnableFlags & ENB_SpecialAttack || SpecialCancel && HasHit && IsAttacking)
			return true;
		break;
	case EStateType::SuperAttack:
		if (EnableFlags & ENB_SuperAttack || SuperCancel && HasHit && IsAttacking)
			return true;
		break;
	case EStateType::Tech:
		if (EnableFlags & ENB_Tech)
			return true;
		break;
	case EStateType::Burst:
		if (!Enemy->Player->LockOpponentBurst && !IsDead)
			return true;
		break;
	default:
		return false;
	}
	return false;
}

void APlayerCharacter::EnableState(EEnableFlags NewEnableFlags)
{
	EnableFlags |= (int)NewEnableFlags;	
}

void APlayerCharacter::DisableState(EEnableFlags NewEnableFlags)
{
	EnableFlags = EnableFlags & ~(int)NewEnableFlags;
}

void APlayerCharacter::EnableAll()
{
	EnableState(ENB_Standing);
	EnableState(ENB_Crouching);
	EnableState(ENB_Jumping);
	EnableState(ENB_ForwardWalk);
	EnableState(ENB_BackWalk);
	EnableState(ENB_ForwardDash);
	EnableState(ENB_BackDash);
	EnableState(ENB_ForwardAirDash);
	EnableState(ENB_BackAirDash);
	EnableState(ENB_NormalAttack);
	EnableState(ENB_SpecialAttack);
	EnableState(ENB_SuperAttack);
	EnableState(ENB_Block);
	EnableState(ENB_Parry);
	DisableState(ENB_Tech);
}

void APlayerCharacter::DisableGroundMovement()
{
	DisableState(ENB_Standing);
	DisableState(ENB_Crouching);
	DisableState(ENB_ForwardWalk);
	DisableState(ENB_BackWalk);
	DisableState(ENB_ForwardDash);
	DisableState(ENB_BackDash);
	DisableState(ENB_Tech);
}

void APlayerCharacter::DisableAll()
{
	DisableState(ENB_Standing);
	DisableState(ENB_Crouching);
	DisableState(ENB_Jumping);
	DisableState(ENB_ForwardWalk);
	DisableState(ENB_BackWalk);
	DisableState(ENB_ForwardDash);
	DisableState(ENB_BackDash);
	DisableState(ENB_ForwardAirDash);
	DisableState(ENB_BackAirDash);
	DisableState(ENB_NormalAttack);
	DisableState(ENB_SpecialAttack);
	DisableState(ENB_SuperAttack);
	DisableState(ENB_Block);
	DisableState(ENB_Parry);
	DisableState(ENB_Tech);
}

bool APlayerCharacter::CheckInputRaw(EInputFlags Input)
{
	if (Inputs & Input)
		return true;
	return false;
}

bool APlayerCharacter::CheckIsStunned()
{
	return IsStunned;
}

void APlayerCharacter::RemoveStun()
{
	Hitstun = -1;
	Blockstun = -1;
	Untech = -1;
	KnockdownTime = -1;
	DisableState(ENB_Tech);
}

void APlayerCharacter::AddAirJump(int NewAirJump)
{
	CurrentAirJumpCount += NewAirJump;
}

void APlayerCharacter::AddAirDash(int NewAirDash)
{
	CurrentAirDashCount += NewAirDash;
}

bool APlayerCharacter::HandleStateCondition(EStateCondition StateCondition)
{
	switch(StateCondition)
	{
	case EStateCondition::None:
		return true;
	case EStateCondition::AirJumpOk:
		if (CurrentAirJumpCount > 0)
			return true;
		break;
	case EStateCondition::AirJumpMinimumHeight:
		if (SpeedY <= 0 || PosY >= 122500)
			return true;
		break;
	case EStateCondition::AirDashOk:
		if (CurrentAirDashCount > 0)
			return true;
		break;
	case EStateCondition::AirDashMinimumHeight:
		if (PosY > AirDashMinimumHeight && SpeedY > 0)
			return true;
		if (PosY > 70000 && SpeedY <= 0)
			return true;
		break;
	case EStateCondition::IsAttacking:
		return IsAttacking;
	case EStateCondition::HitstopCancel:
		return Hitstop == 0 && IsAttacking;
	case EStateCondition::IsStunned:
		return IsStunned;
	case EStateCondition::CloseNormal:
		if (abs(PosX - Enemy->PosX) < CloseNormalRange && !FarNormalForceEnable)
			return true;
		break;
	case EStateCondition::FarNormal:
		if (abs(PosX - Enemy->PosX) > CloseNormalRange || FarNormalForceEnable)
			return true;
		break;
	case EStateCondition::MeterNotZero:
		if (GameState->BattleState.Meter[PlayerIndex] > 0)
			return true;
		break;
	case EStateCondition::MeterQuarterBar:
		if (GameState->BattleState.Meter[PlayerIndex] >= 2500)
			return true;
		break;
	case EStateCondition::MeterHalfBar:
		if (GameState->BattleState.Meter[PlayerIndex] >= 5000)
			return true;
		break;
	case EStateCondition::MeterOneBar:
		if (GameState->BattleState.Meter[PlayerIndex] >= 10000)
			return true;
		break;
	case EStateCondition::MeterTwoBars:
		if (GameState->BattleState.Meter[PlayerIndex] >= 20000)
			return true;
		break;
	case EStateCondition::MeterThreeBars:
		if (GameState->BattleState.Meter[PlayerIndex] >= 30000)
			return true;
		break;
	case EStateCondition::MeterFourBars:
		if (GameState->BattleState.Meter[PlayerIndex] >= 40000)
			return true;
		break;
	case EStateCondition::MeterFiveBars:
		if (GameState->BattleState.Meter[PlayerIndex] >= 50000)
			return true;
		break;
	case EStateCondition::UniversalGaugeOneBar:
		if (GameState->BattleState.UniversalGauge[PlayerIndex] >= 10000)
			return true;
		break;
	case EStateCondition::UniversalGaugeTwoBars:
		if (GameState->BattleState.UniversalGauge[PlayerIndex] >= 20000)
			return true;
		break;
	case EStateCondition::UniversalGaugeThreeBars:
		if (GameState->BattleState.UniversalGauge[PlayerIndex] >= 30000)
			return true;
		break;
	case EStateCondition::UniversalGaugeFourBars:
		if (GameState->BattleState.UniversalGauge[PlayerIndex] >= 40000)
			return true;
		break;
	case EStateCondition::UniversalGaugeFiveBars:
		if (GameState->BattleState.UniversalGauge[PlayerIndex] >= 50000)
			return true;
		break;
	case EStateCondition::UniversalGaugeSixBars:
		if (GameState->BattleState.UniversalGauge[PlayerIndex] >= 60000)
			return true;
		break;
	default:
		return false;
	}
	return false;
}

void APlayerCharacter::SetAirDashTimer(bool IsForward)
{
	if (IsForward)
	{
		AirDashTimer = AirDashTimerMax = FAirDashTime;
	}
	else
	{
		AirDashTimer = AirDashTimerMax = BAirDashTime;
	}
}

void APlayerCharacter::SetAirDashNoAttackTimer(bool IsForward)
{
	if (IsForward)
	{
		AirDashNoAttackTime = FAirDashNoAttackTime;
	}
	else
	{
		AirDashNoAttackTime = BAirDashNoAttackTime;
	}
}

bool APlayerCharacter::CheckInput(FInputCondition Input)
{
	return InputBuffer.CheckInputCondition(Input);
}

void APlayerCharacter::EnableAttacks()
{
	EnableState(ENB_NormalAttack);
	EnableState(ENB_SpecialAttack);
	EnableState(ENB_SuperAttack);
}

void APlayerCharacter::HandleHitAction()
{
	for (int i = 0; i < 32; i++)
	{
		if (IsValid(ChildBattleActors[i]))
		{
			if (ChildBattleActors[i]->DeactivateOnReceiveHit)
			{
				ChildBattleActors[i]->DeactivateObject();
			}
		}
	}
	if (CurrentHealth <= 0)
	{
		IsDead = true;
		if (PosY <= 0)
		{
			JumpToState("Crumple");
		}
		else
		{
			if (ReceivedHitAction == HACT_AirFaceUp)
				JumpToState("BLaunch");
			else if (ReceivedHitAction == HACT_AirVertical)
				JumpToState("VLaunch");
			else if (ReceivedHitAction == HACT_AirFaceDown)
				JumpToState("FLaunch");
			else if (ReceivedHitAction == HACT_Blowback)
				JumpToState("Blowback");
			else
				JumpToState("BLaunch");
		}
		ReceivedHitAction = HACT_None;
		ReceivedAttackLevel = -1;
		return;
	}
	switch (ReceivedHitAction)
	{
	case HACT_GroundNormal:
		if (ActionFlags == ACT_Standing)
		{
			if (ReceivedAttackLevel == 0)
				JumpToState("Hitstun0");
			else if (ReceivedAttackLevel == 1)
				JumpToState("Hitstun1");
			else if (ReceivedAttackLevel == 2)
				JumpToState("Hitstun2");
			else if (ReceivedAttackLevel == 3)
				JumpToState("Hitstun3");
			else if (ReceivedAttackLevel == 4)
				JumpToState("Hitstun4");
		}
		else if (ActionFlags == ACT_Crouching)
		{
			if (ReceivedAttackLevel == 0)
				JumpToState("CrouchHitstun0");
			else if (ReceivedAttackLevel == 1)
				JumpToState("CrouchHitstun1");
			else if (ReceivedAttackLevel == 2)
				JumpToState("CrouchHitstun2");
			else if (ReceivedAttackLevel == 3)
				JumpToState("CrouchHitstun3");
			else if (ReceivedAttackLevel == 4)
				JumpToState("CrouchHitstun4");
			Hitstun += 2;
		}
		break;
	case HACT_AirNormal:
		JumpToState("BLaunch");
		break;
	case HACT_Crumple:
		JumpToState("Crumple");
		break;
	case HACT_ForceCrouch:
		ActionFlags = ACT_Crouching;
		if (ReceivedAttackLevel == 0)
			JumpToState("CrouchHitstun0");
		else if (ReceivedAttackLevel == 1)
			JumpToState("CrouchHitstun1");
		else if (ReceivedAttackLevel == 2)
			JumpToState("CrouchHitstun2");
		else if (ReceivedAttackLevel == 3)
			JumpToState("CrouchHitstun3");
		else if (ReceivedAttackLevel == 4)
			JumpToState("CrouchHitstun4");
		Hitstun += 2;
		break;
	case HACT_ForceStand:
		ActionFlags = ACT_Standing;
		if (ReceivedAttackLevel == 0)
			JumpToState("Hitstun0");
		else if (ReceivedAttackLevel == 1)
			JumpToState("Hitstun1");
		else if (ReceivedAttackLevel == 2)
			JumpToState("Hitstun2");
		else if (ReceivedAttackLevel == 3)
			JumpToState("Hitstun3");
		else if (ReceivedAttackLevel == 4)
			JumpToState("Hitstun4");
		break;
	case HACT_GuardBreakCrouch:
		JumpToState("GuardBreakCrouch");
		break;
	case HACT_GuardBreakStand:
		JumpToState("GuardBreak");
		break;
	case HACT_AirFaceUp:
		JumpToState("BLaunch");
		break;
	case HACT_AirVertical:
		JumpToState("VLaunch");
		break;
	case HACT_AirFaceDown:
		JumpToState("FLaunch");
		break;
	case HACT_Blowback:
		JumpToState("Blowback");
		break;
	case HACT_None: break;
	default: ;
	}
	EnableInertia();
	DisableAll();
	ReceivedHitAction = HACT_None;
	ReceivedAttackLevel = -1;
}

bool APlayerCharacter::IsCorrectBlock(EBlockType BlockType)
{
	if (BlockType != BLK_None)
	{
		FInputCondition Left;
		FInputBitmask BitmaskLeft;
		BitmaskLeft.InputFlag = InputLeft;
		Left.Sequence.Add(BitmaskLeft);
		Left.bInputAllowDisable = false;
		Left.Lenience = 12;
		FInputCondition Right;
		FInputBitmask BitmaskRight;
		BitmaskRight.InputFlag = InputRight;
		Right.Sequence.Add(BitmaskRight);
		if (CheckInput(Left) && !CheckInput(Right) && PosY > 0 || GetCurrentStateName() == "AirBlock")
		{
			Left.Method = EInputMethod::Once;
			if (CheckInput(Left) && InstantBlockTimer <= 0)
			{
				AddMeter(800);
				AddUniversalGauge(500);
				AddColor = FLinearColor(3,3,3,1);
				AddFadeSpeed = 100;
				MulColor = FLinearColor(0.5,0.2,0.8,1);
				MulFadeSpeed = 100;
			}
			return true;
		}
		FInputCondition Input1;
		FInputBitmask BitmaskDownLeft;
		BitmaskDownLeft.InputFlag = InputDownLeft;
		Input1.Sequence.Add(BitmaskDownLeft);
		Input1.Method = EInputMethod::Strict;
		Input1.bInputAllowDisable = false;
		Input1.Lenience = 12;
		if ((CheckInput(Input1) || GetCurrentStateName() == "CrouchBlock") && BlockType != BLK_High && !CheckInput(Right))
		{
			Input1.Method = EInputMethod::OnceStrict;
			if (CheckInput(Input1) && InstantBlockTimer <= 0)
			{
				AddMeter(800);
				AddUniversalGauge(500);
				AddColor = FLinearColor(3,3,3,1);
				AddFadeSpeed = 100;
			}
			return true;
		}
		FInputCondition Input4;
		Input4.Sequence.Add(BitmaskLeft);
		Input4.Method = EInputMethod::Strict;
		Input4.bInputAllowDisable = false;
		Input4.Lenience = 12;
		if ((CheckInput(Input4) || GetCurrentStateName() == "Block") && BlockType != BLK_Low && !CheckInput(Right))
		{
			Input4.Method = EInputMethod::OnceStrict;
			if (CheckInput(Input4) && InstantBlockTimer <= 0)
			{
				AddMeter(800);
				AddUniversalGauge(500);
				AddColor = FLinearColor(3,3,3,1);
				AddFadeSpeed = 100;
			}
			return true;
		}
	}
	return false;
}

bool APlayerCharacter::IsCorrectParry(EBlockType BlockType)
{
	if (ParryTimer > 0)
		return false;
	
	FInputCondition Left;
	FInputBitmask BitmaskLeft;
	BitmaskLeft.InputFlag = InputLeft;
	Left.Sequence.Add(BitmaskLeft);
	FInputCondition Right;
	FInputBitmask BitmaskRight;
	BitmaskRight.InputFlag = InputRight;
	Right.Sequence.Add(BitmaskRight);
	Right.Lenience = 10;
	Right.Method = EInputMethod::Once;
	Right.bInputAllowDisable = false;
	if (CheckInput(Right) && PosY > 0 && !CheckInput(Left))
	{
		return true;
	}
	FInputCondition Input3;
	FInputBitmask BitmaskDownRight;
	BitmaskDownRight.InputFlag = InputDownRight;
	Input3.Sequence.Add(BitmaskDownRight);
	Input3.Lenience = 10;
	Input3.Method = EInputMethod::OnceStrict;
	Input3.bInputAllowDisable = false;
	if (CheckInput(Input3) && BlockType != BLK_Mid && BlockType != BLK_High && !CheckInput(Left))
	{
		return true;
	}
	FInputCondition Input6;
	Input6.Sequence.Add(BitmaskRight);
	Input6.Lenience = 10;
	Input6.Method = EInputMethod::OnceStrict;
	Input6.bInputAllowDisable = false;
	if (CheckInput(Input6) && BlockType != BLK_Low && !CheckInput(Left))
	{
		return true;
	}
	return false;
}

void APlayerCharacter::CheckMissedInstantBlock()
{
	FInputCondition Left;
	FInputBitmask BitmaskLeft;
	BitmaskLeft.InputFlag = InputLeft;
	Left.Sequence.Add(BitmaskLeft);
	Left.bInputAllowDisable = false;
	Left.Lenience = 12;
	if (CheckInput(Left))
	{
		Left.Method = EInputMethod::Once;
		if (!CheckInput(Left))
		{
			InstantBlockTimer = 30;
		}
	}
}

void APlayerCharacter::CheckMissedParry()
{
	FInputCondition Right;
	FInputBitmask BitmaskRight;
	BitmaskRight.InputFlag = InputRight;
	Right.Sequence.Add(BitmaskRight);
	Right.bInputAllowDisable = false;
	Right.Lenience = 10;
	if (CheckInput(Right))
	{
		Right.Method = EInputMethod::Once;
		if (!CheckInput(Right))
		{
			ParryTimer = 12;
		}
	}
}

void APlayerCharacter::HandleBlockAction(EBlockType BlockType)
{
	EnableInertia();
	FInputCondition Input1;
	FInputBitmask BitmaskDownLeft;
	BitmaskDownLeft.InputFlag = InputDownLeft;
	Input1.Sequence.Add(BitmaskDownLeft);
	Input1.Method = EInputMethod::Strict;
	FInputCondition Left;
	FInputBitmask BitmaskLeft;
	BitmaskLeft.InputFlag = InputLeft;
	Left.Sequence.Add(BitmaskLeft);
	if ((CheckInput(Left) && PosY > 0) || GetCurrentStateName() == "AirBlock")
	{
		JumpToState("AirBlock");
		ActionFlags = ACT_Jumping;
	}
	else if ((CheckInput(Input1) && PosY <= 0) || GetCurrentStateName() == "CrouchBlock")
	{
		JumpToState("CrouchBlock");
		ActionFlags = ACT_Crouching;
	}
	else 
	{
		JumpToState("Block");
		ActionFlags = ACT_Standing;
	}
}

void APlayerCharacter::EnableJumpCancel(bool Enable)
{
	JumpCancel = Enable;
}

void APlayerCharacter::EnableBAirDashCancel(bool Enable)
{
	BAirDashCancel = Enable;
}

void APlayerCharacter::EnableChainCancel(bool Enable)
{
	ChainCancelEnabled = Enable;
}

void APlayerCharacter::EnableWhiffCancel(bool Enable)
{
	WhiffCancelEnabled = Enable;
}

void APlayerCharacter::EnableSpecialCancel(bool Enable)
{
	SpecialCancel = Enable;
	SuperCancel = Enable;
}

void APlayerCharacter::EnableSuperCancel(bool Enable)
{
	SuperCancel = Enable;
}

void APlayerCharacter::SetDefaultLandingAction(bool Enable)
{
	DefaultLandingAction = Enable;
}

void APlayerCharacter::SetStrikeInvulnerable(bool Invulnerable)
{
	StrikeInvulnerable = Invulnerable;
}

void APlayerCharacter::SetThrowInvulnerable(bool Invulnerable)
{
	ThrowInvulnerable = Invulnerable;
}

void APlayerCharacter::SetStrikeInvulnerableForTime(int32 Timer)
{
	StrikeInvulnerableForTime = Timer;
}

void APlayerCharacter::SetThrowInvulnerableForTime(int32 Timer)
{
	ThrowInvulnerableForTime = Timer;
}

void APlayerCharacter::SetProjectileInvulnerable(bool Invulnerable)
{
	ProjectileInvulnerable = Invulnerable;
}

void APlayerCharacter::SetHeadInvulnerable(bool Invulnerable)
{
	HeadInvulnerable = Invulnerable;
}

void APlayerCharacter::ForceEnableFarNormal(bool Enable)
{
	FarNormalForceEnable = Enable;
}

void APlayerCharacter::SetThrowActive(bool Active)
{
	ThrowActive = Active;
}

void APlayerCharacter::ThrowExe()
{
	JumpToState(ExeStateName.GetString());
	ThrowActive = false;
}

void APlayerCharacter::ThrowEnd()
{
	if (!Enemy) return;
	Enemy->IsThrowLock = false;
}

void APlayerCharacter::SetThrowRange(int32 InThrowRange)
{
	ThrowRange = InThrowRange;
}

void APlayerCharacter::SetThrowExeState(FString ExeState)
{
	ExeStateName.SetString(ExeState);
}

void APlayerCharacter::SetThrowPosition(int32 ThrowPosX, int32 ThrowPosY)
{
	if (!Enemy) return;
	if (FacingRight)
		Enemy->PosX = R + ThrowPosX;
	else
		Enemy->PosX = L - ThrowPosX;
	Enemy->PosY = PosY + ThrowPosY;
}

void APlayerCharacter::SetThrowLockCel(int32 Index)
{
	if (Index < Enemy->ThrowLockCels.Num())
	{
		Enemy->SetCelName(Enemy->ThrowLockCels[Index]);
	}
}

void APlayerCharacter::PlayVoice(FString Name)
{
	if (VoiceData != nullptr)
	{
		for (FSoundDataStruct SoundStruct : VoiceData->SoundDatas)
		{
			if (SoundStruct.Name == Name)
			{
				GameState->PlayVoiceLine(SoundStruct.SoundWave, SoundStruct.MaxDuration, ObjNumber - 400);
				break;
			}
		}
	}
}

ABattleActor* APlayerCharacter::AddBattleActor(FString InStateName, int PosXOffset, int PosYOffset, EPosType PosType)
{
	int StateIndex = ObjectStateNames.Find(InStateName);
	if (StateIndex != INDEX_NONE)
	{
		int32 FinalPosX, FinalPosY;
		if (!FacingRight)
			PosXOffset = -PosXOffset;

		switch (PosType)
		{
		case POS_Player:
		case POS_Self:
			FinalPosX = PosX + PosXOffset;
			FinalPosY = PosY + PosYOffset;
			break;
		case POS_Enemy:
			FinalPosX = Enemy->PosX + PosXOffset;
			FinalPosY = Enemy->PosY + PosYOffset;
			break;
		case POS_Hit:
			FinalPosX = HitPosX + PosXOffset;
			FinalPosY = HitPosY + PosYOffset;
			break;
		default:
			FinalPosX = PosX + PosXOffset;
			FinalPosY = PosY + PosYOffset;
			break;
		}
		for (int i = 0; i < 32; i++)
		{
			if (ChildBattleActors[i] == nullptr)
			{
				ChildBattleActors[i] = GameState->AddBattleActor(ObjectStates[StateIndex],
					FinalPosX, FinalPosY, FacingRight, this);
				return ChildBattleActors[i];
			}
			if (!ChildBattleActors[i]->IsActive)
			{
				ChildBattleActors[i] = GameState->AddBattleActor(ObjectStates[StateIndex],
					FinalPosX, FinalPosY, FacingRight, this);
				return ChildBattleActors[i];
			}
		}
	}
	return nullptr;
}

ABattleActor* APlayerCharacter::AddCommonBattleActor(FString InStateName, int32 PosXOffset, int32 PosYOffset,
	EPosType PosType)
{
	int StateIndex = CommonObjectStateNames.Find(InStateName);
	if (StateIndex != INDEX_NONE)
	{
		int32 FinalPosX, FinalPosY;
		if (!FacingRight)
			PosXOffset = -PosXOffset;

		switch (PosType)
		{
		case POS_Player:
		case POS_Self:
			FinalPosX = PosX + PosXOffset;
			FinalPosY = PosY + PosYOffset;
			break;
		case POS_Enemy:
			FinalPosX = Enemy->PosX + PosXOffset;
			FinalPosY = Enemy->PosY + PosYOffset;
			break;
		case POS_Hit:
			FinalPosX = HitPosX + PosXOffset;
			FinalPosY = HitPosY + PosYOffset;
			break;
		default:
			FinalPosX = PosX + PosXOffset;
			FinalPosY = PosY + PosYOffset;
			break;
		}
		for (int i = 0; i < 32; i++)
		{
			if (ChildBattleActors[i] == nullptr)
			{
				ChildBattleActors[i] = GameState->AddBattleActor(CommonObjectStates[StateIndex],
					FinalPosX, FinalPosY, FacingRight, this);
				return ChildBattleActors[i];
			}
			if (!ChildBattleActors[i]->IsActive)
			{
				ChildBattleActors[i] = GameState->AddBattleActor(CommonObjectStates[StateIndex],
					FinalPosX, FinalPosY, FacingRight, this);
				return ChildBattleActors[i];
			}
		}
	}
	return nullptr;
}

void APlayerCharacter::AddBattleActorToStorage(ABattleActor* InActor, int Index)
{
	if (Index < 16)
	{
		StoredBattleActors[Index] = InActor;
	}
}

void APlayerCharacter::ToggleComponentVisibility(FString ComponentName, bool Visible)
{
	TInlineComponentArray<USceneComponent*> Components;
	GetComponents(Components);
	for (int i = 0; i < Components.Num(); i++)
	{
		USceneComponent* Component = Components[i];
		if (Component->GetName() == ComponentName)
		{
			ComponentVisible[i] = Visible;
		}
	}
}

void APlayerCharacter::EmptyStateMachine()
{
	StateMachine.States.Empty();
	StateMachine.StateNames.Empty();
	StateMachine.CurrentState = nullptr;
}

bool APlayerCharacter::IsEnemyHitboxWithinRange(int32 Range)
{
	for (int i = 0; i < GameState->ActiveObjectCount; i++)
	{
		if (ABattleActor* Actor = GameState->SortedObjects[i]; Actor->Player != Player)
		{
			for (FCollisionBoxInternal Box : Actor->CollisionBoxesInternal)
			{
				if (Box.Type == Hitbox)
				{
					if (abs(PosX - (Box.PosX + Box.SizeX / 2)) < Range && abs(PosY - (Box.PosY + Box.SizeX / 2)) < Range)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

#if WITH_EDITOR
void APlayerCharacter::EditorUpdate()
{
	int TempAnimTime = AnimTime;
	AnimTime = 0;
	AnimBPTime = 0;
	for (int i = 0; i <= TempAnimTime; i++)
	{
		AnimTime++;
		AnimBPTime++;
		StateMachine.Tick(0.01666666666666);
		GetBoxes();
	}
}
#endif

void APlayerCharacter::PlayCommonLevelSequence(FString Name)
{
	if (CommonSequenceData != nullptr)
	{
		for (FSequenceStruct SequenceStruct : CommonSequenceData->SequenceDatas)
		{
			if (SequenceStruct.Name == Name)
			{
				GameState->PlayLevelSequence(this, SequenceStruct.Sequence);
			}
		}
	}
}

void APlayerCharacter::PlayLevelSequence(FString Name)
{
	if (SequenceData != nullptr)
	{
		for (FSequenceStruct SequenceStruct : SequenceData->SequenceDatas)
		{
			if (SequenceStruct.Name == Name)
			{
				GameState->PlayLevelSequence(this, SequenceStruct.Sequence);
			}
		}
	}
}

void APlayerCharacter::EnableFAirDashCancel(bool Enable)
{
	FAirDashCancel = Enable;
}

void APlayerCharacter::AddChainCancelOption(FString Option)
{
	ChainCancelOptions.Add(Option);
	if (ChainCancelOptions.Num() > 0)
	{
		ChainCancelOptionsInternal[ChainCancelOptions.Num() - 1] = StateMachine.GetStateIndex(Option);
	}
}

void APlayerCharacter::AddWhiffCancelOption(FString Option)
{
	WhiffCancelOptions.Add(Option);
	if (WhiffCancelOptions.Num() > 0)
	{
		WhiffCancelOptionsInternal[WhiffCancelOptions.Num() - 1] = StateMachine.GetStateIndex(Option);
	}
}

bool APlayerCharacter::FindChainCancelOption(FString Name)
{
	if (HasHit && IsAttacking && ChainCancelEnabled)
	{
		for (int i = 0; i < CancelArraySize; i++)
		{
			if (ChainCancelOptionsInternal[i] == StateMachine.GetStateIndex(Name) && ChainCancelOptionsInternal[i] != INDEX_NONE)
			{
				return true;
			}
		}
	}
	return false;
}

bool APlayerCharacter::FindWhiffCancelOption(FString Name)
{
	if (WhiffCancelEnabled)
	{
		for (int i = 0; i < CancelArraySize; i++)
		{
			if (WhiffCancelOptionsInternal[i] == StateMachine.GetStateIndex(Name) && WhiffCancelOptionsInternal[i] != INDEX_NONE)
			{
				return true;
			}
		}
	}
	return false;
}

void APlayerCharacter::SetComponentVisibility()
{
	TInlineComponentArray<USceneComponent*> Components;
	GetComponents(Components);
	for (int i = 0; i < Components.Num(); i++)
	{
		USceneComponent* Component = Components[i];
		Component->SetVisibility(ComponentVisible[i]);
	}
}

void APlayerCharacter::CalculateUltra()
{
	//calculate ultra factor
	if (SpeedX > 0 && !IsStunned)
	{
		UltraFactor += 25;
	}
	else if (IsAttacking)
	{
		UltraFactor += 50;
	}
	else
	{
		UltraFactor -= 50;
	}
	if (abs(PosX - Enemy->PosX) >= 1080000)
	{
		UltraFactor -= 75;
	}
	if (UltraFactor > 5000)
		UltraFactor = 5000;
	if (UltraFactor < -1000)
		UltraFactor = -1000;

	//add ultra factor to gauge
	if (UltraFactor > 0)
	{
		GameState->BattleState.UniversalGauge[PlayerIndex] += UltraFactor / 100;
		
		//ultra gauge cap
		if (GameState->BattleState.UniversalGauge[PlayerIndex] > GameState->BattleState.MaxUniversalGauge)
		{
			GameState->BattleState.UniversalGauge[PlayerIndex] = GameState->BattleState.MaxUniversalGauge;
		}
		if (GameState->BattleState.UniversalGauge[PlayerIndex] < 0)
		{
			GameState->BattleState.UniversalGauge[PlayerIndex] = 0;
		}
	}
}

void APlayerCharacter::StartSuperFreeze(int Duration)
{
	GameState->StartSuperFreeze(Duration);
	StateMachine.CurrentState->OnSuperFreeze();
}

void APlayerCharacter::BattleHudVisibility(bool Visible)
{
	GameState->BattleHudVisibility(Visible);
}

void APlayerCharacter::DisableLastInput()
{
	InputBuffer.InputDisabled[89] = InputBuffer.InputBufferInternal[89];
}

void APlayerCharacter::OnStateChange()
{
	for (int i = 0; i < 32; i++)
	{
		if (IsValid(ChildBattleActors[i]))
		{
			if (ChildBattleActors[i]->DeactivateOnStateChange)
			{
				ChildBattleActors[i]->DeactivateObject();
			}
		}
	}
	DisableLastInput();
	if (MiscFlags & MISC_FlipEnable)
		HandleFlip();
	SetDefaultComponentVisibility();
	EnableKaraCancel = true;
	ProrateOnce = false;
	ChainCancelOptions.Empty();
	WhiffCancelOptions.Empty();
	StateName.SetString("");
	HitEffectName.SetString("");
	for (int i = 0; i < CancelArraySize; i++)
	{
		ChainCancelOptionsInternal[i] = -1;
		WhiffCancelOptionsInternal[i] = -1;
	}
	ChainCancelEnabled = true;
	WhiffCancelEnabled = false;
	JumpCancel = false;
	FAirDashCancel = false;
	BAirDashCancel = false;
	HasHit = false;
	AnimTime = 0; //reset anim time
	AnimBPTime = 0; //reset animbp time
	ActionTime = 0; //reset action time
	DefaultLandingAction = true;
	DefaultCommonAction = true;
	FarNormalForceEnable = false;
	SpeedXPercent = 100;
	SpeedXPercentPerFrame = false;
	SpeedYPercent = 100;
	SpeedYPercentPerFrame = false;
	IsAttacking = false;
	ThrowActive = false;
	StrikeInvulnerable = false;
	ThrowInvulnerable = false;
	ProjectileInvulnerable = false;
	HeadInvulnerable = false;
	AttackHeadAttribute = false;
	AttackProjectileAttribute = false;
	PushWidthExpand = 0;
	LoopCounter = 0;
	StateVal1 = 0;
	StateVal2 = 0;
	StateVal3 = 0;
	StateVal4 = 0;
	StateVal5 = 0;
	StateVal6 = 0;
	StateVal7 = 0;
	StateVal8 = 0;
	FlipInputs = false;
	PushCollisionActive = true;
	LockOpponentBurst = false;
}

void APlayerCharacter::SaveForRollbackPlayer(unsigned char* Buffer)
{
	FMemory::Memcpy(Buffer, &PlayerSync, SIZEOF_PLAYERCHARACTER);
}

void APlayerCharacter::LoadForRollbackPlayer(unsigned char* Buffer)
{
	FMemory::Memcpy(&PlayerSync, Buffer, SIZEOF_PLAYERCHARACTER);
	for (int i = 0; i < CancelArraySize; i++) //reload TArrays with rolled back data
	{
		ChainCancelOptions.Empty();
		WhiffCancelOptions.Empty();
		if (StateMachine.StateNames.Num() > 0)
		{
			if (ChainCancelOptionsInternal[i] != -1)
			{
				ChainCancelOptions.Add(StateMachine.GetStateName(ChainCancelOptionsInternal[i]));
			}
			if (WhiffCancelOptionsInternal[i] != -1)
			{
				WhiffCancelOptions.Add(StateMachine.GetStateName(WhiffCancelOptionsInternal[i]));
			}
		}
	}
}

void APlayerCharacter::HandleThrowCollision()
{
	if (IsAttacking && ThrowActive && !Enemy->ThrowInvulnerable && !Enemy->ThrowInvulnerableForTime && !Enemy->GetInternalValue(VAL_IsStunned)
		&& (Enemy->PosY <= 0 && PosY <= 0 && Enemy->KnockdownTime < 0 || Enemy->PosY > 0 && PosY > 0))
	{
		int ThrowPosX;
		if (FacingRight)
			ThrowPosX = R + ThrowRange;
		else
			ThrowPosX = L - ThrowRange;
		if ((PosX <= Enemy->PosX && ThrowPosX >= Enemy->L || PosX > Enemy->PosX && ThrowPosX <= Enemy->R)
			&& T >= Enemy->B && B <= Enemy->T)
		{
			Enemy->JumpToState("Hitstun0");
			Enemy->IsThrowLock = true;
			Enemy->ThrowTechTimer = 10;
			ThrowExe();
		}
	}
}

bool APlayerCharacter::CheckKaraCancel(EStateType InStateType)
{
	if (!EnableKaraCancel)
		return false;
	//two checks: if it's an attack, and if the given state type has a higher or equal priority to the current state
	if (InStateType == EStateType::NormalThrow && StateMachine.CurrentState->StateType < InStateType
		&& StateMachine.CurrentState->StateType >= EStateType::NormalAttack && ActionTime < 3
		&& ComboTimer == 0)
	{
		return true;
	}
	if (InStateType == EStateType::SpecialAttack && StateMachine.CurrentState->StateType < InStateType
		&& StateMachine.CurrentState->StateType >= EStateType::NormalAttack && ActionTime < 3)
	{
		return true;
	}
	if (InStateType == EStateType::SuperAttack && StateMachine.CurrentState->StateType < InStateType
		&& StateMachine.CurrentState->StateType >= EStateType::SpecialAttack && ActionTime < 3)
	{
		return true;
	}	
	return false;
}

bool APlayerCharacter::CheckObjectPreventingState(int InObjectID)
{
	for (int i = 0; i < 32; i++)
	{
		if (ChildBattleActors[i] != nullptr)
		{
			if (ChildBattleActors[i]->IsActive)
			{
				if (ChildBattleActors[i]->ObjectID == InObjectID && ChildBattleActors[i]->ObjectID != 0)
					return true;
			}
		}
	}
	return false;
}

void APlayerCharacter::ResetForRound()
{
	SetDefaultComponentVisibility();
	PosX = 0;
	PosY = 0;
	PrevPosX = 0;
	PrevPosY = 0;
	SpeedX = 0;
	SpeedY = 0;
	Gravity = 1900;
	Inertia = 0;
	ActionTime = 0;
	PushHeight = 0;
	PushHeightLow = 0;
	PushWidth = 0;
	PushWidthExpand = 0;
	Hitstop = 0;
	L = 0;
	R = 0;
	T = 0;
	B = 0;
	HitEffect = FHitEffect();
	CounterHitEffect = FHitEffect();
	HitActive = false;
	IsAttacking = false;
	AttackHeadAttribute = false;
	AttackProjectileAttribute = false;
	RoundStart = true;
	FacingRight = false;
	HasHit = false;
	SpeedXPercent = 100;
	SpeedXPercentPerFrame = false;
	SpeedYPercent = 100;
	SpeedYPercentPerFrame = false;
	ScreenCollisionActive = true;
	PushCollisionActive = false;
	ProrateOnce = false;
	StateVal1 = 0;
	StateVal2 = 0;
	StateVal3 = 0;
	StateVal4 = 0;
	StateVal5 = 0;
	StateVal6 = 0;
	StateVal7 = 0;
	StateVal8 = 0;
	MiscFlags = 0;
	SuperFreezeTime = -1;
	CelNameInternal.SetString("");
	HitEffectName.SetString("");
	AnimTime = 0;
	AnimBPTime = 0;
	HitPosX = 0;
	HitPosY = 0;
	for (int i = 0; i < CollisionArraySize; i++)
	{
		CollisionBoxesInternal[i] = FCollisionBoxInternal();
	}
	CollisionBoxes.Empty();
	CelName = "";
	ObjectID = 0;
	EnableFlags = 0;
	CurrentHealth = 0;
	CurrentAirJumpCount = 0;
	CurrentAirDashCount = 0;
	AirDashTimerMax = 0;
	JumpCancel = false;
	FAirDashCancel = false;
	BAirDashCancel = false;
	SpecialCancel = false;
	SuperCancel = false;
	DefaultLandingAction = true;
	FarNormalForceEnable = false;
	EnableKaraCancel = true;
	LockOpponentBurst = false;
	IsDead = false;
	ThrowRange = 0;
	WallBounceEffect = FWallBounceEffect();
	GroundBounceEffect = FGroundBounceEffect();
	ThrowActive = false;
	IsThrowLock = false;
	IsOnScreen = false;
	DeathCamOverride = false;
	IsKnockedDown = false;
	FlipInputs = false;
	Inputs = 0;
	ActionFlags = 0;
	AirDashTimer = 0;
	Hitstun = -1;
	Blockstun = -1;
	Untech = -1;
	KnockdownTime = -1;
	InstantBlockTimer = -1;
	ParryTimer = -1;
	TotalProration = 10000;
	ComboCounter = 0;
	ComboTimer = 0;
	LoopCounter = 0;
	ThrowTechTimer = 0;
	HasBeenOTG = 0;
	WallTouchTimer = 0;
	TouchingWall = false;
	ChainCancelEnabled = true;
	WhiffCancelEnabled = false;
	StrikeInvulnerable = false;
	WhiffCancelEnabled = false;
	StrikeInvulnerableForTime = 0;
	ThrowInvulnerableForTime = 0;
	StrikeInvulnerable = false;
	ThrowInvulnerable = false;
	ProjectileInvulnerable = false;
	HeadInvulnerable = false;
	RoundWinTimer = 300;
	RoundWinInputLock = false;
	MeterCooldownTimer = 0;
	PlayerVal1 = 0;
	PlayerVal2 = 0;
	PlayerVal3 = 0;
	PlayerVal4 = 0;
	PlayerVal5 = 0;
	PlayerVal6 = 0;
	PlayerVal7 = 0;
	PlayerVal8 = 0;
	for (int i = 0; i < CancelArraySize; i++)
	{
		ChainCancelOptionsInternal[i] = -1;
		WhiffCancelOptionsInternal[i] = -1;
	}
	JumpToState("Stand");
	ExeStateName.SetString("");
	ReceivedHitAction = HACT_None;
	ReceivedAttackLevel = -1;
	for (int i = 0; i < 90; i++)
		InputBuffer.InputBufferInternal[i] = InputNeutral;
	CurrentHealth = Health;
	AttackProjectileAttribute = false;
	DefaultLandingAction = true;
	EnableAll();
	EnableFlip(true);
	StateName.SetString("Stand");
}

void APlayerCharacter::HandleWallBounce()
{
	if (Untech > 0)
	{
		if (WallBounceEffect.WallBounceInCornerOnly)
		{
			if (PosX >= 2160000 || PosX <= -2160000)
			{
				if (WallBounceEffect.WallBounceCount > 0)
				{
					TouchingWall = false;
					WallBounceEffect.WallBounceCount--;
					SetSpeedX(WallBounceEffect.WallBounceXSpeed);
					SetSpeedY(WallBounceEffect.WallBounceYSpeed);
					SetGravity(WallBounceEffect.WallBounceGravity);
					if (WallBounceEffect.WallBounceUntech > 0)
						Untech = WallBounceEffect.WallBounceUntech;
					JumpToState("FLaunch");
				}
			}
			return;
		}
		if (TouchingWall)
		{
			if (WallBounceEffect.WallBounceCount > 0)
			{
				TouchingWall = false;
				WallBounceEffect.WallBounceCount--;
				SetSpeedX(WallBounceEffect.WallBounceXSpeed);
				SetSpeedY(WallBounceEffect.WallBounceYSpeed);
				SetGravity(WallBounceEffect.WallBounceGravity);
				if (WallBounceEffect.WallBounceUntech > 0)
					Untech = WallBounceEffect.WallBounceUntech;
				JumpToState("FLaunch");
			}
		}
	}
}

void APlayerCharacter::HandleGroundBounce()
{
	if (KnockdownTime > 0 || Untech > 0)
	{
		if (GroundBounceEffect.GroundBounceCount > 0)
		{
			GroundBounceEffect.GroundBounceCount--;
			SetInertia(-GroundBounceEffect.GroundBounceXSpeed);
			SetSpeedY(GroundBounceEffect.GroundBounceYSpeed);
			SetGravity(GroundBounceEffect.GroundBounceGravity);
			if (GroundBounceEffect.GroundBounceUntech > 0)
				Untech = GroundBounceEffect.GroundBounceUntech;
			JumpToState("BLaunch");
		}
	}
}

void APlayerCharacter::LogForSyncTest()
{
	if (IsOnScreen)
	{
		Super::LogForSyncTest();
		UE_LOG(LogTemp, Warning, TEXT("EnableFlags: %d"), EnableFlags);
		UE_LOG(LogTemp, Warning, TEXT("CurrentAirJumpCount: %d"), CurrentAirJumpCount);
		UE_LOG(LogTemp, Warning, TEXT("CurrentAirDashCount: %d"), CurrentAirDashCount);
		UE_LOG(LogTemp, Warning, TEXT("AirDashTimerMax: %d"), AirDashTimerMax);
		UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %d"), CurrentHealth);
		UE_LOG(LogTemp, Warning, TEXT("JumpCancel: %d"), JumpCancel);
		UE_LOG(LogTemp, Warning, TEXT("FAirDashCancel: %d"), FAirDashCancel);
		UE_LOG(LogTemp, Warning, TEXT("BAirDashCancel: %d"), BAirDashCancel);
		UE_LOG(LogTemp, Warning, TEXT("SpecialCancel: %d"), SpecialCancel);
		UE_LOG(LogTemp, Warning, TEXT("SuperCancel: %d"), SuperCancel);
		UE_LOG(LogTemp, Warning, TEXT("BAirDashCancel: %d"), DefaultLandingAction);
		UE_LOG(LogTemp, Warning, TEXT("Inputs: %d"), InputBuffer.InputBufferInternal[89]);
		UE_LOG(LogTemp, Warning, TEXT("ActionFlags: %d"), ActionFlags);
		UE_LOG(LogTemp, Warning, TEXT("AirDashTimer: %d"), AirDashTimer);
		UE_LOG(LogTemp, Warning, TEXT("Hitstun: %d"), Hitstun);
		UE_LOG(LogTemp, Warning, TEXT("Untech: %d"), Untech);
		UE_LOG(LogTemp, Warning, TEXT("Untech: %d"), TouchingWall);
		UE_LOG(LogTemp, Warning, TEXT("ChainCancelEnabled: %d"), ChainCancelEnabled);
		UE_LOG(LogTemp, Warning, TEXT("WhiffCancelEnabled: %d"), WhiffCancelEnabled);
		UE_LOG(LogTemp, Warning, TEXT("StrikeInvulnerable: %d"), StrikeInvulnerable);
		UE_LOG(LogTemp, Warning, TEXT("ThrowInvulnerable: %d"), ThrowInvulnerable);
		int ChainCancelChecksum = 0;
		for (int i = 0; i < CancelArraySize; i++)
		{
			ChainCancelChecksum += ChainCancelOptionsInternal[i];
		}
		UE_LOG(LogTemp, Warning, TEXT("ChainCancelOptions: %d"), ChainCancelChecksum);
		int WhiffCancelChecksum = 0;
		for (int i = 0; i < CancelArraySize; i++)
		{
			WhiffCancelChecksum += WhiffCancelOptionsInternal[i];
		}
		UE_LOG(LogTemp, Warning, TEXT("ChainCancelOptions: %d"), WhiffCancelChecksum);
		if (StateMachine.States.Num() != 0)
			UE_LOG(LogTemp, Warning, TEXT("StateName: %s"), StateName.GetString());
	}
}

void APlayerCharacter::LogForSyncTestFile(FILE* file)
{
	Super::LogForSyncTestFile(file);
	if(file)
	{
		fprintf(file,"PlayerCharacter:\n");
		fprintf(file,"\tEnableFlags: %d\n", EnableFlags);
		fprintf(file,"\tCurrentAirJumpCount: %d\n", CurrentAirJumpCount);
		fprintf(file,"\tCurrentAirDashCount: %d\n", CurrentAirDashCount);
		fprintf(file,"\tAirDashTimerMax: %d\n", AirDashTimerMax);
		fprintf(file,"\tCurrentHealth: %d\n", CurrentHealth);
		fprintf(file,"\tJumpCancel: %d\n", JumpCancel);
		fprintf(file,"\tFAirDashCancel: %d\n", FAirDashCancel);
		fprintf(file,"\tBAirDashCancel: %d\n", BAirDashCancel);
		fprintf(file,"\tSpecialCancel: %d\n", SpecialCancel);
		fprintf(file,"\tSuperCancel: %d\n", SuperCancel);
		fprintf(file,"\tBAirDashCancel: %d\n", DefaultLandingAction);
		fprintf(file,"\tInputs: %d\n", InputBuffer.InputBufferInternal[89]);
		fprintf(file,"\tActionFlags: %d\n", ActionFlags);
		fprintf(file,"\tAirDashTimer: %d\n", AirDashTimer);
		fprintf(file,"\tHitstun: %d\n", Hitstun);
		fprintf(file,"\tUntech: %d\n", Untech);
		fprintf(file,"\tUntech: %d\n", TouchingWall);
		fprintf(file,"\tChainCancelEnabled: %d\n", ChainCancelEnabled);
		fprintf(file,"\tWhiffCancelEnabled: %d\n", WhiffCancelEnabled);
		fprintf(file,"\tStrikeInvulnerable: %d\n", StrikeInvulnerable);
		fprintf(file,"\tThrowInvulnerable: %d\n", ThrowInvulnerable);
		int ChainCancelChecksum = 0;
		for (int i = 0; i < 0x20; i++)
		{
			ChainCancelChecksum += ChainCancelOptionsInternal[i];
		}
		fprintf(file,"\tChainCancelOptions: %d\n", ChainCancelChecksum);
		int WhiffCancelChecksum = 0;
		for (int i = 0; i < 0x20; i++)
		{
			WhiffCancelChecksum += WhiffCancelOptionsInternal[i];
		}
		fprintf(file,"\tChainCancelOptions: %d\n", WhiffCancelChecksum);
		if (StateMachine.States.Num() != 0)
			fprintf(file,"\tStateName: %s\n", StateName.GetString());
		fprintf(file,"\tEnemy: %p\n", Enemy);
	}
}
