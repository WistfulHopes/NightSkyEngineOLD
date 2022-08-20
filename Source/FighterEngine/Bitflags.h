// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//bitflag enums

UENUM()
enum EInputFlags
{
	InputUp = 0x1,
	InputDown = 0x2,
	InputLeft = 0x4,
	InputRight = 0x8,
	InputUpLeft = InputUp + InputLeft,
	InputUpRight = InputUp + InputRight,
	InputDownLeft = InputDown + InputLeft,
	InputDownRight = InputDown + InputRight,
	InputNeutral = 0x10,
	InputL = 0x20,
	InputM = 0x40,
	InputH = 0x80,
	InputS = 0x100,
	InputA1 = 0x200,
	InputA2 = 0x400,
};

UENUM()
enum EActionFlags
{
	ACT_Standing = 0x1,
	ACT_Crouching = 0x2,
	ACT_Jumping = 0x4,
};

UENUM()
enum EEnableFlags
{
	ENB_Standing = 0x1,
	ENB_Crouching = 0x2,
	ENB_Jumping = 0x4,
	ENB_ForwardWalk = 0x8,
	ENB_BackWalk = 0x10,
	ENB_ForwardDash = 0x20,
	ENB_BackDash = 0x40,
	ENB_ForwardAirDash = 0x80,
	ENB_BackAirDash = 0x100,
	ENB_NormalAttack = 0x200,
	ENB_SpecialAttack = 0x400,
	ENB_SuperAttack = 0x800,
	ENB_Block = 0x1000,
	ENB_Tech = 0x2000,
};

UENUM()
enum EMiscFlags
{
	MISC_InertiaEnable = 0x1,
	MISC_FlipEnable = 0x2,
};