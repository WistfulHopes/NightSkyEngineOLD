// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "FighterGameState.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter()
{
	InitPlayer();
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
}

void APlayerCharacter::InitPlayer()
{
	CurrentHealth = Health;
	DefaultLandingAction = true;
	EnableAll();
	EnableFlip(true);
	StateName.SetString("Stand");
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	StateMachine = NewObject<UStateMachine>();
	StateMachine->Parent = this;
	Init();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::Update()
{
	Super::Update();
	
	//run input buffer before checking hitstop
	if (!FacingRight) //flip inputs with direction
	{
		const unsigned int Bit1 = (Inputs >> 2) & 1;
		const unsigned int Bit2 = (Inputs >> 3) & 1;
		unsigned int x = (Bit1 ^ Bit2);

		x = x << 2 | x << 3;

		Inputs = Inputs ^ x;
	}
	if (StateMachine->GetCurrentState()->StateType != EStateType::Hitstun)
	{
		TotalProration = 10000;
	}
	if (Enemy->StateMachine->GetCurrentState()->StateType != EStateType::Hitstun)
	{
		ComboCounter = 0;
	}
	if (!Inputs << 27) //if no direction, set neutral input
	{
		Inputs |= (int)InputNeutral;
	}
	else
	{
		Inputs = Inputs & ~(int)InputNeutral; //remove neutral input if directional input
	}
	InputBuffer.Tick(Inputs);
	
	if (Hitstop > -1) //issue where if this number is not less than the one set in BattleActor, .
		return;
	
	AirDashTimer--;
	
	Hitstun--;
	if (Hitstun == 0)
	{
		if (ActionFlags == ACT_Standing)
		{
			JumpToState("Stand");
		}
		else if (ActionFlags == ACT_Crouching)
		{
			JumpToState("Crouch");
		}
	}
	Untech--;
	if (Untech == 0 && PosY > 0)
	{
		EnableState(ENB_Tech);
	}
	else if (PosY <= 0)
	{
		Untech = 0;
	}
	Blockstun--;
	if (Blockstun == 0)
	{
		if (ActionFlags & ACT_Standing)
		{
			JumpToState("Stand");
		}
		else if (ActionFlags & ACT_Crouching)
		{
			JumpToState("Crouch");
		}
		else
		{
			JumpToState("VJump");
		}
	}
	
	if (PosY > 0) //set jumping if above ground
	{
		SetActionFlags(ACT_Jumping);
	}
	else //not jumping if on ground
	{
		SetActionFlags(ACT_Standing);
	}
	
	if (PosY == 0 && PrevPosY != 0) //reset air move counts on ground
	{
		CurrentAirJumpCount = AirJumpCount;
		CurrentAirDashCount = AirDashCount;
		if (DefaultLandingAction)
		{
			JumpToState("JumpLanding");
			CreateCommonParticle("cmn_jumpland_smoke", POS_Player);
		}
		else
		{
			StateMachine->GetCurrentState()->OnLanding();
		}
		DefaultLandingAction = true;
	}
	StateMachine->Tick(0.0166666); //update current state
	HandleStateMachine(); //handle state transitions
}

void APlayerCharacter::HandleStateMachine()
{
	for (int i = StateMachine->States.Num() - 1; i >= 0; i--)
	{
		if (CheckStateEnabled(StateMachine->States[i]->StateType) && !StateMachine->States[i]->IsFollowupState
			|| FindChainCancelOption(StateMachine->States[i]->Name)
			|| FindWhiffCancelOption(StateMachine->States[i]->Name)
			) //check if the state is enabled
		{
			if (StateMachine->CheckStateEntryCondition(StateMachine->States[i]->EntryState, ActionFlags)) //check current character state against entry state condition
			{
				if (StateMachine->States[i]->StateConditions.Num() != 0) //only check state conditions if there are any
				{
					for (int j = 0; j < StateMachine->States[i]->StateConditions.Num(); j++) //iterate over state conditions
					{
						if (HandleStateCondition(StateMachine->States[i]->StateConditions[j])) //check state condition
						{
							if (j == StateMachine->States[i]->StateConditions.Num() - 1) //have all conditions been met?
							{
								for (int v = 0; v < StateMachine->States[i]->InputConditions.Num(); v++) //iterate over input conditions
								{
									if (InputBuffer.CheckInputCondition(StateMachine->States[i]->InputConditions[v])) //check input condition against input buffer
									{
										if (v == StateMachine->States[i]->InputConditions.Num() - 1) //have all conditions been met?
										{
											if (FindChainCancelOption(StateMachine->States[i]->Name)
												|| FindWhiffCancelOption(StateMachine->States[i]->Name)) //if cancel option, allow resetting state
											{
												if (StateMachine->ForceSetState(StateMachine->States[i]->Name)) //if state set successful...
												{
													StateName.SetString(StateMachine->States[i]->Name);
													break; //don't try to enter another state
												}
											}
											else
											{
												if (StateMachine->SetState(StateMachine->States[i]->Name)) //if state set successful...
												{
													StateName.SetString(StateMachine->States[i]->Name);
													break; //don't try to enter another state
												}
											}
										}
										continue;
									}
									break;
								}
								if (StateMachine->States[i]->InputConditions.Num() == 0) //if no input condtions, set state
								{
									if (StateMachine->SetState(StateMachine->States[i]->Name)) //if state set successful...
									{
										StateName.SetString(StateMachine->States[i]->Name);
										break; //don't try to enter another state
									}
								}
							}
							continue;
						}
						break;
					}
				}
				else
				{
					for (int v = 0; v < StateMachine->States[i]->InputConditions.Num(); v++) //iterate over input conditions
					{
						if (InputBuffer.CheckInputCondition(StateMachine->States[i]->InputConditions[v])) //check input condition against input buffer
						{
							if (v == StateMachine->States[i]->InputConditions.Num() - 1) //have all conditions been met?
							{
								if (FindChainCancelOption(StateMachine->States[i]->Name)
									|| FindWhiffCancelOption(StateMachine->States[i]->Name)) //if cancel option, allow resetting state
								{
									if (StateMachine->ForceSetState(StateMachine->States[i]->Name)) //if state set successful...
									{
										StateName.SetString(StateMachine->States[i]->Name);
										break; //don't try to enter another state
									}
								}
								else
								{
									if (StateMachine->SetState(StateMachine->States[i]->Name)) //if state set successful...
									{
										StateName.SetString(StateMachine->States[i]->Name);
										break; //don't try to enter another state
									}
								}
							}
							continue;
						}
						break;
					}
					if (StateMachine->States[i]->InputConditions.Num() == 0) //if no input condtions, set state
					{
						if (StateMachine->SetState(StateMachine->States[i]->Name)) //if state set successful...
						{
							StateName.SetString(StateMachine->States[i]->Name);
							break; //don't try to enter another state
						}
					}
				}
			}
		}
	}
}

void APlayerCharacter::SetActionFlags(EActionFlags ActionFlag)
{
	ActionFlags = (int)ActionFlag;
}

void APlayerCharacter::AddState(FString Name, UState* State)
{
	StateMachine->AddState(Name, State);
}

void APlayerCharacter::JumpToState(FString NewName)
{
	if (StateMachine->ForceSetState(NewName))
		HitEffectName.SetString(NewName);
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
}

bool APlayerCharacter::CheckInputRaw(EInputFlags Input)
{
	if (Inputs & Input)
		return true;
	return false;
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
		{
			return true;
		}
		break;
	case EStateCondition::AirDashOk:
		if (CurrentAirDashCount > 0)
		{
			return true;
		}
		break;
	case EStateCondition::AirActionMinimumHeight:
		if (PosY >= AirDashMinimumHeight)
		{
			return true;
		}
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

bool APlayerCharacter::CheckInput(EInputCondition Input)
{
	return InputBuffer.CheckInputCondition(Input);
}

void APlayerCharacter::EnableAttacks()
{
	EnableState(ENB_NormalAttack);
	EnableState(ENB_SpecialAttack);
	EnableState(ENB_SuperAttack);
}

void APlayerCharacter::HandleHitAction(HitAction HACT, int AttackLevel)
{
	EnableInertia();
	DisableAll();
	if (HACT == HACT_GroundNormal)
	{
		if (AttackLevel == 0)
			JumpToState("Hitstun0");
		else if (AttackLevel == 1)
			JumpToState("Hitstun1");
		else if (AttackLevel == 2)
			JumpToState("Hitstun2");
		else if (AttackLevel == 3)
			JumpToState("Hitstun3");
		else if (AttackLevel == 4)
			JumpToState("Hitstun4");
	}
	else if (HACT == HACT_AirNormal)
	{
		JumpToState("BLaunch");
	}
}

bool APlayerCharacter::IsCorrectBlock(EBlockType BlockType)
{
	if (BlockType != BLK_None)
	{
		if (ActionFlags & ACT_Standing && (CheckInput(EInputCondition::Input_4_Hold) || CheckInput(EInputCondition::Input_4_Press))
			&& BlockType != BLK_Low)
		{
			return true;
		}
		if (ActionFlags & ACT_Crouching && (CheckInput(EInputCondition::Input_1_Hold) || CheckInput(EInputCondition::Input_1_Press))
			&& BlockType != BLK_High)
		{
			return true;
		}
	}
	return false;
}

void APlayerCharacter::HandleBlockAction()
{
	EnableInertia();
	if (CheckInputRaw(InputDownLeft))
	{
		JumpToState("CrouchBlock");
	}
	else if (CheckInputRaw(InputLeft))
	{
		JumpToState("Block");
	}
	else
	{
		JumpToState("AirBlock");
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

void APlayerCharacter::PlayVoice(FString Name)
{
	if (VoiceData != nullptr)
	{
		for (FSoundDataStruct SoundStruct : VoiceData->SoundDatas)
		{
			if (SoundStruct.Name == Name)
			{
				UGameplayStatics::PlaySoundAtLocation(this, SoundStruct.SoundWave, GetActorLocation());
				break;
			}
		}
	}
}

void APlayerCharacter::AddBattleActor(FString InStateName, int PosXOffset, int PosYOffset)
{
	int StateIndex = ObjectStateNames.Find(InStateName);
	if (StateIndex != INDEX_NONE)
	{
		if (!FacingRight)
			PosXOffset = -PosXOffset;
		Cast<AFighterGameState>(GetWorld()->GetGameState())->AddBattleActor(ObjectStates[StateIndex], PosX + PosXOffset, PosY + PosYOffset, FacingRight, this);
	}
}

void APlayerCharacter::PlayCommonLevelSequence(FString Name)
{
	AFighterGameState* GameState = Cast<AFighterGameState>(GetWorld()->GetGameState());
	if (!FacingRight)
		Name.Append("_Flip");
	if (CommonSequenceData != nullptr)
	{
		for (FSequenceStruct SequenceStruct : CommonSequenceData->SequenceDatas)
		{
			if (SequenceStruct.Name == Name)
			{
				GameState->PlayLevelSequence(SequenceStruct.Sequence);
			}
		}
	}
}

void APlayerCharacter::PlayLevelSequence(FString Name)
{
	AFighterGameState* GameState = Cast<AFighterGameState>(GetWorld()->GetGameState());
	if (!FacingRight)
		Name.Append("_Flip");
	if (SequenceData != nullptr)
	{
		for (FSequenceStruct SequenceStruct : SequenceData->SequenceDatas)
		{
			if (SequenceStruct.Name == Name)
			{
				GameState->PlayLevelSequence(SequenceStruct.Sequence);
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
		ChainCancelOptionsInternal[ChainCancelOptions.Num() - 1] = StateMachine->GetStateIndex(Option);
	}
}

void APlayerCharacter::AddWhiffCancelOption(FString Option)
{
	WhiffCancelOptions.Add(Option);
	if (WhiffCancelOptions.Num() > 0)
	{
		WhiffCancelOptionsInternal[WhiffCancelOptions.Num() - 1] = StateMachine->GetStateIndex(Option);
	}
}

bool APlayerCharacter::FindChainCancelOption(FString Name)
{
	if (HasHit && IsAttacking && ChainCancelEnabled)
	{
		for (int i = 0; i < CancelArraySize; i++)
		{
			if (ChainCancelOptionsInternal[i] == StateMachine->GetStateIndex(Name) && ChainCancelOptionsInternal[i] != INDEX_NONE)
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
			if (WhiffCancelOptionsInternal[i] == StateMachine->GetStateIndex(Name) && WhiffCancelOptionsInternal[i] != INDEX_NONE)
			{
				return true;
			}
		}
	}
	return false;
}

void APlayerCharacter::OnStateChange()
{
	ChainCancelOptions.Empty();
	WhiffCancelOptions.Empty();
	StateName.SetString("");
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
	AnimTime = -1; //reset anim time
	AnimBPTime = -1; //reset animbp time
	ActionTime = -1; //reset action time
	DefaultLandingAction = true;
}

void APlayerCharacter::SaveForRollbackPlayer(unsigned char* Buffer)
{
	FMemory::Memcpy(Buffer, &PlayerSync, SIZEOF_PLAYERCHARACTER);
}

void APlayerCharacter::LoadForRollbackPlayer(unsigned char* Buffer)
{
	FMemory::Memcpy(&PlayerSync, Buffer, SIZEOF_PLAYERCHARACTER);
	if (StateMachine->States.Num() != 0)
		StateMachine->ForceRollbackState(StateName.GetString());
	for (int i = 0; i < CancelArraySize; i++) //reload TArrays with rolled back data
	{
		ChainCancelOptions.Empty();
		WhiffCancelOptions.Empty();
		if (StateMachine->StateNames.Num() > 0)
		{
			if (ChainCancelOptionsInternal[i] != -1)
			{
				ChainCancelOptions.Add(StateMachine->GetStateName(ChainCancelOptionsInternal[i]));
			}
			if (WhiffCancelOptionsInternal[i] != -1)
			{
				WhiffCancelOptions.Add(StateMachine->GetStateName(WhiffCancelOptionsInternal[i]));
			}
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
		if (StateMachine->States.Num() != 0)
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
		fprintf(file,"\tEnemy: %p\n", Enemy);
	}
}
