#pragma once

UENUM()
enum EInputFlags
{
	UInputUp = 0x1,
	UInputDown = 0x2,
	UInputLeft = 0x4,
	UInputRight = 0x8,
	UInputUpLeft = UInputUp + UInputLeft,
	UInputUpRight = UInputUp + UInputRight,
	UInputDownLeft = UInputDown + UInputLeft,
	UInputDownRight = UInputDown + UInputRight,
	UInputNeutral = 0x10,
	UInputL = 0x20,
	UInputM = 0x40,
	UInputH = 0x80,
	UInputS = 0x100,
	UInputA1 = 0x200,
	UInputA2 = 0x400,
};

UENUM()
enum EActionFlags
{
	UACT_Standing = 0x1,
	UACT_Crouching = 0x2,
	UACT_Jumping = 0x4,
};

UENUM()
enum EEnableFlags
{
	UENB_Standing = 0x1,
	UENB_Crouching = 0x2,
	UENB_Jumping = 0x4,
	UENB_ForwardWalk = 0x8,
	UENB_BackWalk = 0x10,
	UENB_ForwardDash = 0x20,
	UENB_BackDash = 0x40,
	UENB_ForwardAirDash = 0x80,
	UENB_BackAirDash = 0x100,
	UENB_NormalAttack = 0x200,
	UENB_SpecialAttack = 0x400,
	UENB_SuperAttack = 0x800,
	UENB_Block = 0x1000,
	UENB_Tech = 0x2000,
};

UENUM()
enum EMiscFlags
{
	UMISC_InertiaEnable = 0x1,
	UMISC_FlipEnable = 0x2,
};