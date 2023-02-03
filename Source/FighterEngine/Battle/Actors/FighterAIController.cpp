// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterAIController.h"
#include "FighterEngine/Battle/Bitflags.h"

void AFighterAIController::PressUp()
{
	Inputs |= InputUp;
}

void AFighterAIController::ReleaseUp()
{
	Inputs = Inputs & ~InputUp;
}

void AFighterAIController::PressDown()
{
	Inputs |= InputDown;
}

void AFighterAIController::ReleaseDown()
{
	Inputs = Inputs & ~InputDown;
}

void AFighterAIController::PressLeft()
{
	Inputs |= InputLeft;
}

void AFighterAIController::ReleaseLeft()
{
	Inputs = Inputs & ~InputLeft;
}

void AFighterAIController::PressRight()
{
	Inputs |= InputRight;
}

void AFighterAIController::ReleaseRight()
{
	Inputs = Inputs & ~InputRight;
}

void AFighterAIController::ReleaseAllDirections()
{
	Inputs = Inputs & ~InputUp;
	Inputs = Inputs & ~InputDown;
	Inputs = Inputs & ~InputLeft;
	Inputs = Inputs & ~InputRight;
}

void AFighterAIController::PressL()
{
	Inputs |= InputL;
}

void AFighterAIController::ReleaseL()
{
	Inputs = Inputs & ~InputL;
}

void AFighterAIController::PressM()
{
	Inputs |= InputM;
}

void AFighterAIController::ReleaseM()
{
	Inputs = Inputs & ~InputM;
}

void AFighterAIController::PressH()
{
	Inputs |= InputH;
}

void AFighterAIController::ReleaseH()
{
	Inputs = Inputs & ~InputH;
}

void AFighterAIController::PressS()
{
	Inputs |= InputS;
}

void AFighterAIController::ReleaseS()
{
	Inputs = Inputs & ~InputS;
}

void AFighterAIController::ReleaseAllButtons()
{
	Inputs = Inputs & ~InputL;
	Inputs = Inputs & ~InputM;
	Inputs = Inputs & ~InputH;
	Inputs = Inputs & ~InputS;
}
