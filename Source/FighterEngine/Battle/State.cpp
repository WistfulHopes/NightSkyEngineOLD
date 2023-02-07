// Fill out your copyright notice in the Description page of Project Settings.


#include "State.h"

#include "ScriptAnalyzer.h"
#include "Actors/PlayerCharacter.h"

void UState::OnEnter_Implementation()
{
	if (auto ScriptState = Cast<UNightSkyScriptState>(this); IsValid(ScriptState))
	{
		if (ScriptState->ParentState)
		{
			ScriptState->ParentState->OnEnter();
		}
		if (ScriptState->Offsets.OnEnterOffset == -1)
			return;
		if (ScriptState->CommonState)
		{
			Parent->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnEnterOffset), Parent);
			return;
		}
		if (Parent)
		{
			Parent->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnEnterOffset), Parent);
		}
		else
		{
			ObjectParent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnEnterOffset), Parent);
		}
	}
}

void UState::OnUpdate_Implementation(float DeltaTime)
{
	if (auto ScriptState = Cast<UNightSkyScriptState>(this); IsValid(ScriptState))
	{
		if (ScriptState->ParentState)
		{
			ScriptState->ParentState->OnUpdate(DeltaTime);
		}
		if (ScriptState->Offsets.OnUpdateOffset == -1)
			return;
		if (ScriptState->CommonState)
		{
			Parent->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnUpdateOffset), Parent);
			return;
		}
		if (Parent)
		{
			Parent->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnUpdateOffset), Parent);
		}
		else
		{
			ObjectParent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnUpdateOffset), Parent);
		}
	}
}

void UState::OnExit_Implementation()
{
	if (auto ScriptState = Cast<UNightSkyScriptState>(this); IsValid(ScriptState))
	{
		if (ScriptState->ParentState)
		{
			ScriptState->ParentState->OnExit();
		}
		if (ScriptState->Offsets.OnExitOffset == -1)
			return;
		if (ScriptState->CommonState)
		{
			Parent->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnExitOffset), Parent);
			return;
		}
		if (Parent)
		{
			Parent->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnExitOffset), Parent);
		}
		else
		{
			ObjectParent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnExitOffset), Parent);
		}
	}
}

void UState::OnLanding_Implementation()
{
	if (auto ScriptState = Cast<UNightSkyScriptState>(this); IsValid(ScriptState))
	{
		if (ScriptState->ParentState)
		{
			ScriptState->ParentState->OnLanding();
		}
		if (ScriptState->Offsets.OnLandingOffset == -1)
			return;
		if (ScriptState->CommonState)
		{
			Parent->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnLandingOffset), Parent);
			return;
		}
		if (Parent)
		{
			Parent->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnLandingOffset), Parent);
		}
		else
		{
			ObjectParent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnLandingOffset), Parent);
		}
	}
}

void UState::OnHit_Implementation()
{
	if (auto ScriptState = Cast<UNightSkyScriptState>(this); IsValid(ScriptState))
	{
		if (ScriptState->ParentState)
		{
			ScriptState->ParentState->OnHit();
		}
		if (ScriptState->Offsets.OnHitOffset == -1)
			return;
		if (ScriptState->CommonState)
		{
			Parent->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnHitOffset), Parent);
			return;
		}
		if (Parent)
		{
			Parent->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnHitOffset), Parent);
		}
		else
		{
			ObjectParent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnHitOffset), Parent);
		}
	}
}

void UState::OnBlock_Implementation()
{
	if (auto ScriptState = Cast<UNightSkyScriptState>(this); IsValid(ScriptState))
	{
		if (ScriptState->ParentState)
		{
			ScriptState->ParentState->OnBlock();
		}
		if (ScriptState->Offsets.OnBlockOffset == -1)
			return;
		if (ScriptState->CommonState)
		{
			Parent->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnBlockOffset), Parent);
			return;
		}
		if (Parent)
		{
			Parent->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnBlockOffset), Parent);
		}
		else
		{
			ObjectParent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnBlockOffset), Parent);
		}
	}
}

void UState::OnHitOrBlock_Implementation()
{
	if (auto ScriptState = Cast<UNightSkyScriptState>(this); IsValid(ScriptState))
	{
		if (ScriptState->ParentState)
		{
			ScriptState->ParentState->OnHitOrBlock();
		}
		if (ScriptState->Offsets.OnHitOrBlockOffset == -1)
			return;
		if (ScriptState->CommonState)
		{
			Parent->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnHitOrBlockOffset), Parent);
			return;
		}
		if (Parent)
		{
			Parent->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnHitOrBlockOffset), Parent);
		}
		else
		{
			ObjectParent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnHitOrBlockOffset), Parent);
		}
	}
}

void UState::OnCounterHit_Implementation()
{
	if (auto ScriptState = Cast<UNightSkyScriptState>(this); IsValid(ScriptState))
	{
		if (ScriptState->ParentState)
		{
			ScriptState->ParentState->OnCounterHit();
		}
		if (ScriptState->Offsets.OnCounterHitOffset == -1)
			return;
		if (ScriptState->CommonState)
		{
			Parent->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnCounterHitOffset), Parent);
			return;
		}
		if (Parent)
		{
			Parent->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnCounterHitOffset), Parent);
		}
		else
		{
			ObjectParent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnCounterHitOffset), Parent);
		}
	}
}

void UState::OnSuperFreeze_Implementation()
{
	if (auto ScriptState = Cast<UNightSkyScriptState>(this); IsValid(ScriptState))
	{
		if (ScriptState->ParentState)
		{
			ScriptState->ParentState->OnSuperFreeze();
		}
		if (ScriptState->Offsets.OnSuperFreezeOffset == -1)
			return;
		if (ScriptState->CommonState)
		{
			Parent->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnSuperFreezeOffset), Parent);
			return;
		}
		if (Parent)
		{
			Parent->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnSuperFreezeOffset), Parent);
		}
		else
		{
			ObjectParent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnSuperFreezeOffset), Parent);
		}
	}
}

void UState::OnSuperFreezeEnd_Implementation()
{
	if (auto ScriptState = Cast<UNightSkyScriptState>(this); IsValid(ScriptState))
	{
		if (ScriptState->ParentState)
		{
			ScriptState->ParentState->OnSuperFreezeEnd();
		}
		if (ScriptState->Offsets.OnSuperFreezeEndOffset == -1)
			return;
		if (ScriptState->CommonState)
		{
			Parent->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnSuperFreezeEndOffset), Parent);
			return;
		}
		if (Parent)
		{
			Parent->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnSuperFreezeEndOffset), Parent);
		}
		else
		{
			ObjectParent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptState->Offsets.OnSuperFreezeEndOffset), Parent);
		}
	}
}
