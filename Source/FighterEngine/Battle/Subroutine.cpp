// Fill out your copyright notice in the Description page of Project Settings.


#include "Subroutine.h"

#include "ScriptAnalyzer.h"
#include "Actors/BattleActor.h"
#include "Actors/PlayerCharacter.h"

void USubroutine::OnCall_Implementation()
{
	if (auto ScriptSubroutine = Cast<UNightSkyScriptSubroutine>(this); IsValid(ScriptSubroutine))
	{
		if (ScriptSubroutine->CommonSubroutine)
		{
			Parent->Player->CommonAnalyzer.Analyze(reinterpret_cast<char*>(ScriptSubroutine->OffsetAddress), Parent);
			return;
		}
		if (Parent)
		{
			Parent->Player->CharaAnalyzer.Analyze(reinterpret_cast<char*>(ScriptSubroutine->OffsetAddress), Parent);
		}
	}
}
