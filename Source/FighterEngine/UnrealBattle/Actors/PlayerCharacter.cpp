// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Battle/Actors/PlayerCharacter.h"

#include "FighterGameState.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter()
{
	Player = this;
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
	StandPushWidth = 110000;
	StandPushHeight = 240000;
	CrouchPushWidth = 120000;
	CrouchPushHeight = 180000;
	AirPushWidth = 100000;
	AirPushHeight = 275000;
	AirPushHeightLow = -135000;
	DefaultCommonAction = true;
	Health = 10000;
	ForwardWalkMeterGain = 12;
	ForwardJumpMeterGain = 10;
	ForwardDashMeterGain = 25;
	ForwardAirDashMeterGain = 25;
}

void APlayerCharacter::Init()
{
	Update();
	for (FString Cel : ThrowLockCels)
	{
		CString<64> CName;
		CName.SetString(TCHAR_TO_ANSI(*Cel));
		reinterpret_cast<PlayerCharacter*>(Parent.Get())->ThrowLockCels.push_back(CName);
	}
}

void APlayerCharacter::Update()
{
	Super::Update();

	reinterpret_cast<PlayerCharacter*>(Parent.Get())->FWalkSpeed = FWalkSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->BWalkSpeed = BWalkSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->FDashInitSpeed = FDashInitSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->FDashAccel = FDashAccel;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->FDashMaxSpeed = FDashMaxSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->FDashFriction = FDashFriction;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->BDashSpeed = BDashSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->BDashHeight = BDashHeight;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->BDashGravity = BDashGravity;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->JumpHeight = JumpHeight;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->FJumpSpeed = FJumpSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->BJumpSpeed = BJumpSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->JumpGravity = JumpGravity;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SuperJumpHeight = SuperJumpHeight;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->FSuperJumpSpeed = FSuperJumpSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->BSuperJumpSpeed = BSuperJumpSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SuperJumpGravity = SuperJumpGravity;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirDashMinimumHeight = AirDashMinimumHeight;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->FAirDashSpeed = FAirDashSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->BAirDashSpeed = BAirDashSpeed;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->FAirDashTime = FAirDashTime;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->BAirDashTime = BAirDashTime;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->FAirDashNoAttackTime = FAirDashNoAttackTime;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->BAirDashNoAttackTime = BAirDashNoAttackTime;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirJumpCount = AirJumpCount;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirDashCount = AirDashCount;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->StandPushWidth = StandPushWidth;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->StandPushHeight = StandPushHeight;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->CrouchPushWidth = CrouchPushWidth;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->CrouchPushHeight = CrouchPushHeight;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirPushWidth = AirPushWidth;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirPushHeight = AirPushHeight;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirPushHeightLow = AirPushHeightLow;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->CloseNormalRange = CloseNormalRange;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->Health = Health;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->ComboRate = ComboRate;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->ForwardWalkMeterGain = ForwardWalkMeterGain;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->ForwardJumpMeterGain = ForwardJumpMeterGain;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->ForwardDashMeterGain = ForwardDashMeterGain;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->ForwardAirDashMeterGain = ForwardAirDashMeterGain;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->MeterPercentOnHit = MeterPercentOnHit;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->MeterPercentOnHitGuard = MeterPercentOnHitGuard;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->MeterPercentOnReceiveHitGuard = MeterPercentOnReceiveHitGuard;
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->MeterPercentOnReceiveHit = MeterPercentOnReceiveHit;
}

void APlayerCharacter::OnLoadGameState()
{
	Super::OnLoadGameState();
	
	FWalkSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->FWalkSpeed;
	BWalkSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->BWalkSpeed;
	FDashInitSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->FDashInitSpeed;
	FDashAccel = reinterpret_cast<PlayerCharacter*>(Parent.Get())->FDashAccel;
	FDashMaxSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->FDashMaxSpeed;
	FDashFriction = reinterpret_cast<PlayerCharacter*>(Parent.Get())->FDashFriction;
	BDashSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->BDashSpeed;
	BDashHeight = reinterpret_cast<PlayerCharacter*>(Parent.Get())->BDashHeight;
	BDashGravity = reinterpret_cast<PlayerCharacter*>(Parent.Get())->BDashGravity;
	JumpHeight = reinterpret_cast<PlayerCharacter*>(Parent.Get())->JumpHeight;
	FJumpSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->FJumpSpeed;
	BJumpSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->BJumpSpeed;
	JumpGravity = reinterpret_cast<PlayerCharacter*>(Parent.Get())->JumpGravity;
	SuperJumpHeight = reinterpret_cast<PlayerCharacter*>(Parent.Get())->SuperJumpHeight;
	FSuperJumpSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->FSuperJumpSpeed;
	BSuperJumpSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->BSuperJumpSpeed;
	SuperJumpGravity = reinterpret_cast<PlayerCharacter*>(Parent.Get())->SuperJumpGravity;
	AirDashMinimumHeight = reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirDashMinimumHeight;
	FAirDashSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->FAirDashSpeed;
	BAirDashSpeed = reinterpret_cast<PlayerCharacter*>(Parent.Get())->BAirDashSpeed;
	FAirDashTime = reinterpret_cast<PlayerCharacter*>(Parent.Get())->FAirDashTime;
	BAirDashTime = reinterpret_cast<PlayerCharacter*>(Parent.Get())->BAirDashTime;
	FAirDashNoAttackTime = reinterpret_cast<PlayerCharacter*>(Parent.Get())->FAirDashNoAttackTime;
	BAirDashNoAttackTime = reinterpret_cast<PlayerCharacter*>(Parent.Get())->BAirDashNoAttackTime;
	AirJumpCount = reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirJumpCount;
	AirDashCount = reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirDashCount;
	StandPushWidth = reinterpret_cast<PlayerCharacter*>(Parent.Get())->StandPushWidth;
	StandPushHeight = reinterpret_cast<PlayerCharacter*>(Parent.Get())->StandPushHeight;
	CrouchPushWidth = reinterpret_cast<PlayerCharacter*>(Parent.Get())->CrouchPushWidth;
	CrouchPushHeight = reinterpret_cast<PlayerCharacter*>(Parent.Get())->CrouchPushHeight;
	AirPushWidth = reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirPushWidth;
	AirPushHeight = reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirPushHeight;
	AirPushHeightLow = reinterpret_cast<PlayerCharacter*>(Parent.Get())->AirPushHeightLow;
	CloseNormalRange = reinterpret_cast<PlayerCharacter*>(Parent.Get())->CloseNormalRange;
	Health = reinterpret_cast<PlayerCharacter*>(Parent.Get())->Health;
	ComboRate = reinterpret_cast<PlayerCharacter*>(Parent.Get())->ComboRate;
	ForwardWalkMeterGain = reinterpret_cast<PlayerCharacter*>(Parent.Get())->ForwardWalkMeterGain;
	ForwardJumpMeterGain = reinterpret_cast<PlayerCharacter*>(Parent.Get())->ForwardJumpMeterGain;
	ForwardDashMeterGain = reinterpret_cast<PlayerCharacter*>(Parent.Get())->ForwardDashMeterGain;
	ForwardAirDashMeterGain = reinterpret_cast<PlayerCharacter*>(Parent.Get())->ForwardAirDashMeterGain;
	MeterPercentOnHit = reinterpret_cast<PlayerCharacter*>(Parent.Get())->MeterPercentOnHit;
	MeterPercentOnHitGuard = reinterpret_cast<PlayerCharacter*>(Parent.Get())->MeterPercentOnHitGuard;
	MeterPercentOnReceiveHitGuard = reinterpret_cast<PlayerCharacter*>(Parent.Get())->MeterPercentOnReceiveHitGuard;
	MeterPercentOnReceiveHit = reinterpret_cast<PlayerCharacter*>(Parent.Get())->MeterPercentOnReceiveHit;
}

void APlayerCharacter::SetParent(PlayerCharacter* InActor)
{
	Parent = TSharedPtr<PlayerCharacter>(InActor);
}

void APlayerCharacter::SetActionFlags(EActionFlags ActionFlag)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetActionFlags((ActionFlags)ActionFlag);
}

void APlayerCharacter::AddState(FString Name, UState* State)
{
	State->Parent = this;
	State->ParentState = TUniquePtr<BlueprintState>(new BlueprintState(State));
	State->ParentState->Name.SetString(TCHAR_TO_ANSI(*State->Name));
	State->ParentState->StateEntryState = (EntryState)State->EntryState;
	for (auto InInputCondition : State->InputConditions)
		State->ParentState->InputConditions.push_back((InputCondition)InInputCondition);
	for (auto InStateCondition : State->StateConditions)
		State->ParentState->StateConditions.push_back((StateCondition)InStateCondition);
	State->ParentState->Type = (StateType)State->StateType;
	State->ParentState->IsFollowupState = State->IsFollowupState;
	State->ParentState->ObjectID = State->ObjectID;
	States.Add(State);
	StateNames.Add(Name);
	CString<64> CName;
	CName.SetString(TCHAR_TO_ANSI(*Name));
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AddState(CName, State->ParentState.Get());
}

void APlayerCharacter::AddSubroutine(FString Name, USubroutine* Subroutine)
{
	Subroutine->Parent = this;
	Subroutine->ParentSubroutine = TUniquePtr<BlueprintSubroutine>(new BlueprintSubroutine(Subroutine));
	Subroutine->ParentSubroutine->Name.SetString(TCHAR_TO_ANSI(*Subroutine->Name));
	Subroutines.Add(Subroutine);
	SubroutineNames.Add(Name);
	CString<64> CName;
	CName.SetString(TCHAR_TO_ANSI(*Name));
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AddSubroutine(CName, Subroutine->ParentSubroutine.Get());
}

void APlayerCharacter::CallSubroutine(FString Name)
{
	CString<64> CName;
	CName.SetString(TCHAR_TO_ANSI(*Name));
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->CallSubroutine(TCHAR_TO_ANSI(*Name));
}

void APlayerCharacter::UseMeter(int Use)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->UseMeter(Use);
}

void APlayerCharacter::AddMeter(int Meter)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AddMeter(Meter);
}

void APlayerCharacter::SetMeterCooldownTimer(int Timer)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetMeterCooldownTimer(Timer);
}

void APlayerCharacter::JumpToState(FString NewName)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->JumpToState(TCHAR_TO_ANSI(*NewName));
}

FString APlayerCharacter::GetCurrentStateName()
{
	return reinterpret_cast<PlayerCharacter*>(Parent.Get())->GetCurrentStateName().GetString();
}

int32 APlayerCharacter::GetLoopCount()
{
	return reinterpret_cast<PlayerCharacter*>(Parent.Get())->GetLoopCount();
}

void APlayerCharacter::IncrementLoopCount()
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->IncrementLoopCount();
}

bool APlayerCharacter::CheckStateEnabled(EStateType InStateType)
{
	return reinterpret_cast<PlayerCharacter*>(Parent.Get())->CheckStateEnabled((StateType)InStateType);
}

void APlayerCharacter::EnableState(EEnableFlags NewEnableFlags)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->EnableState((EnableFlags)NewEnableFlags);
}

void APlayerCharacter::DisableState(EEnableFlags NewEnableFlags)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->DisableState((EnableFlags)NewEnableFlags);
}

void APlayerCharacter::EnableAll()
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->EnableAll();
}

void APlayerCharacter::DisableGroundMovement()
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->DisableGroundMovement();
}

void APlayerCharacter::DisableAll()
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->DisableAll();
}

bool APlayerCharacter::CheckInputRaw(EInputFlags Input)
{
	return reinterpret_cast<PlayerCharacter*>(Parent.Get())->CheckInputRaw((InputFlags)Input);
}

bool APlayerCharacter::CheckIsStunned()
{
	return reinterpret_cast<PlayerCharacter*>(Parent.Get())->CheckIsStunned();
}


void APlayerCharacter::AddAirJump(int NewAirJump)
{
	return reinterpret_cast<PlayerCharacter*>(Parent.Get())->AddAirJump(NewAirJump);
}

void APlayerCharacter::AddAirDash(int NewAirDash)
{
	return reinterpret_cast<PlayerCharacter*>(Parent.Get())->AddAirDash(NewAirDash);
}

void APlayerCharacter::SetAirDashTimer(bool IsForward)
{
	return reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetAirDashTimer(IsForward);
}

bool APlayerCharacter::CheckInput(EInputCondition Input)
{
	return reinterpret_cast<PlayerCharacter*>(Parent.Get())->CheckInput((InputCondition)Input);
}

void APlayerCharacter::EnableAttacks()
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->EnableAttacks();
}

void APlayerCharacter::EnableJumpCancel(bool Enable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->EnableJumpCancel(Enable);
}

void APlayerCharacter::EnableBAirDashCancel(bool Enable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->EnableBAirDashCancel(Enable);
}

void APlayerCharacter::EnableChainCancel(bool Enable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->EnableChainCancel(Enable);
}

void APlayerCharacter::EnableWhiffCancel(bool Enable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->EnableWhiffCancel(Enable);
}

void APlayerCharacter::EnableSpecialCancel(bool Enable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->EnableSpecialCancel(Enable);
}

void APlayerCharacter::EnableSuperCancel(bool Enable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->EnableSuperCancel(Enable);
}

void APlayerCharacter::SetDefaultLandingAction(bool Enable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetDefaultLandingAction(Enable);
}

void APlayerCharacter::SetStrikeInvulnerable(bool Invulnerable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetStrikeInvulnerable(Invulnerable);
}

void APlayerCharacter::SetThrowInvulnerable(bool Invulnerable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetThrowInvulnerable(Invulnerable);
}

void APlayerCharacter::SetHeadInvulnerable(bool Invulnerable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetHeadInvulnerable(Invulnerable);
}

void APlayerCharacter::ForceEnableFarNormal(bool Enable)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->ForceEnableFarNormal(Enable);
}

void APlayerCharacter::SetThrowActive(bool Active)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetThrowActive(Active);
}

void APlayerCharacter::ThrowEnd()
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->ThrowEnd();
}

void APlayerCharacter::SetThrowRange(int32 InThrowRange)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetThrowRange(InThrowRange);
}

void APlayerCharacter::SetThrowExeState(FString ExeState)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetThrowExeState(TCHAR_TO_ANSI(*ExeState));
}

void APlayerCharacter::SetThrowPosition(int32 ThrowPosX, int32 ThrowPosY)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetThrowPosition(ThrowPosX, ThrowPosY);
}

void APlayerCharacter::SetThrowLockCel(int32 Index)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->SetThrowLockCel(Index);
}

void APlayerCharacter::PlayVoice(FString Name)
{
	if (VoiceData != nullptr)
	{
		for (FSoundDataStruct SoundStruct : VoiceData->SoundDatas)
		{
			if (SoundStruct.Name == Name)
			{
				GameState->PlayVoiceLine(SoundStruct.SoundWave, SoundStruct.MaxDuration, reinterpret_cast<PlayerCharacter*>(Parent.Get())->ObjNumber - 400);
				break;
			}
		}
	}
}

ABattleActor* APlayerCharacter::AddBattleActor(FString InStateName, int PosXOffset, int PosYOffset)
{
	BattleActor* TmpActor = reinterpret_cast<PlayerCharacter*>(Parent.Get())->AddBattleActor(TCHAR_TO_ANSI(*InStateName), PosXOffset, PosYOffset);
	for (int i = 0; i < 400; i++)
	{
		if (GameState->Objects[i]->GetParent() == TmpActor)
		{
			return GameState->Objects[i];
		}
	}
	return nullptr;
}

void APlayerCharacter::AddBattleActorToStorage(ABattleActor* InActor, int Index)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AddBattleActorToStorage(InActor->GetParent(), Index);
}

void APlayerCharacter::EditorUpdate()
{
	int TempAnimTime = AnimTime;
	AnimTime = -1;
	AnimBPTime = -1;
	for (int i = 0; i <= TempAnimTime; i++)
	{
		AnimTime++;
		AnimBPTime++;
		reinterpret_cast<PlayerCharacter*>(Parent.Get())->StateMachine.Tick(0.01666666666666);
		GetBoxes();
	}
}

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
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->EnableFAirDashCancel(Enable);
}

void APlayerCharacter::AddChainCancelOption(FString Option)
{
	CString<64> Name;
	Name.SetString(TCHAR_TO_ANSI(*Option));
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AddChainCancelOption(Name);
}

void APlayerCharacter::AddWhiffCancelOption(FString Option)
{
	CString<64> Name;
	Name.SetString(TCHAR_TO_ANSI(*Option));
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->AddWhiffCancelOption(Name);
}

void APlayerCharacter::StartSuperFreeze(int Duration)
{
	reinterpret_cast<PlayerCharacter*>(Parent.Get())->StartSuperFreeze(Duration);
}

void APlayerCharacter::BattleHudVisibility(bool Visible)
{
	GameState->BattleHudVisibility(Visible);
}