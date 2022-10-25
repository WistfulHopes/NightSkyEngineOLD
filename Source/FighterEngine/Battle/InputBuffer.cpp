// Fill out your copyright notice in the Description page of Project Settings.


#include "InputBuffer.h"
#include "Bitflags.h"

void FInputBuffer::Tick(int32 Input)
{
	for (int32 i = 0; i < 89; i++)
	{
		InputBufferInternal[i] = InputBufferInternal[i + 1];
		InputDisabled[i] = InputDisabled[i + 1];
	}
	InputBufferInternal[89] = Input;
	InputDisabled[89] = false;
}

bool FInputBuffer::CheckInputCondition(const EInputCondition InputCondition)
{
	Lenience = 8;
	ImpreciseInputCount = 0;
	switch (InputCondition)
	{
	case EInputCondition::None:
		if (InputBufferInternal[89] & InputNeutral)
		{
			return true;
		}
		break;
	case EInputCondition::Input_1:
		ResetInputSequence();
		InputSequence[0] = InputDownLeft;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_1_Press:
		if (InputBufferInternal[89] & InputDown && InputBufferInternal[89] & InputLeft
			&& !(InputBufferInternal[86] & InputDown && InputBufferInternal[86] & InputLeft))
		{
			return true;
		}
		break;
	case EInputCondition::Input_1_Hold:
		if (InputBufferInternal[89] & InputDown && InputBufferInternal[89] & InputLeft
			&& InputBufferInternal[88] & InputDown && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case EInputCondition::Input_1_Release:
		if (!(InputBufferInternal[89] & InputDown && InputBufferInternal[89] & InputLeft)
			&& InputBufferInternal[88] & InputDown && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case EInputCondition::Input_2:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_2_Press:
		if (InputBufferInternal[89] & InputDown && !(InputBufferInternal[89] & InputLeft)
			&& !(InputBufferInternal[89] & InputRight) && !(InputBufferInternal[86] & InputDown))
		{
			return true;
		}
		break;
	case EInputCondition::Input_2_Hold:
		if (InputBufferInternal[89] & InputDown && !(InputBufferInternal[89] & InputLeft)
			&& !(InputBufferInternal[89] & InputRight) && InputBufferInternal[88] & InputDown
			&& !(InputBufferInternal[89] & InputUp))
		{
			return true;
		}
		break;
	case EInputCondition::Input_2_Release:
		if (!(InputBufferInternal[89] & InputDown) && InputBufferInternal[86] & InputDown)
		{
			return true;
		}
		break;
	case EInputCondition::Input_3:
		ResetInputSequence();
		InputSequence[0] = InputDownRight;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_3_Press:
		if (InputBufferInternal[89] & InputDown && InputBufferInternal[89] & InputRight
			&& !(InputBufferInternal[86] & InputDown && InputBufferInternal[86] & InputRight))
		{
			return true;
		}
		break;
	case EInputCondition::Input_3_Hold:
		if (InputBufferInternal[89] & InputDown && InputBufferInternal[89] & InputRight
			&& InputBufferInternal[88] & InputDown && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case EInputCondition::Input_3_Release:
		if (!(InputBufferInternal[89] & InputDown && InputBufferInternal[89] & InputRight)
			&& InputBufferInternal[88] & InputDown && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case EInputCondition::Input_4:
		ResetInputSequence();
		InputSequence[0] = InputLeft;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_4_Press:
		if (InputBufferInternal[89] & InputLeft && !(InputBufferInternal[89] & InputUp)
			&& !(InputBufferInternal[89] & InputDown) && !(InputBufferInternal[86] & InputLeft))
		{
			return true;
		}
		break;
	case EInputCondition::Input_4_Hold:
		if (InputBufferInternal[89] & InputLeft && !(InputBufferInternal[89] & InputUp)
			&& !(InputBufferInternal[89] & InputDown) && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case EInputCondition::Input_4_Release:
		if (!(InputBufferInternal[89] & InputLeft) && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case EInputCondition::Input_6:
		ResetInputSequence();
		InputSequence[0] = InputRight;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_6_Press:
		if (InputBufferInternal[89] & InputRight && !(InputBufferInternal[89] & InputUp)
			&& !(InputBufferInternal[89] & InputDown) && !(InputBufferInternal[86] & InputRight))
		{
			return true;
		}
		break;
	case EInputCondition::Input_6_Hold:
		if (InputBufferInternal[89] & InputRight && !(InputBufferInternal[89] & InputUp)
			&& !(InputBufferInternal[89] & InputDown) && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case EInputCondition::Input_6_Release:
		if (!(InputBufferInternal[89] & InputRight) && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case EInputCondition::Input_7:
		ResetInputSequence();
		InputSequence[0] = InputUpLeft;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_7_Press:
		if (InputBufferInternal[89] & InputUp && InputBufferInternal[89] & InputLeft
			&& !(InputBufferInternal[86] & InputUp && InputBufferInternal[86] & InputLeft))
		{
			return true;
		}
		break;
	case EInputCondition::Input_7_Hold:
		if (InputBufferInternal[89] & InputUp && InputBufferInternal[89] & InputLeft
			&& InputBufferInternal[88] & InputUp && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case EInputCondition::Input_7_Release:
		if (!(InputBufferInternal[89] & InputUp && InputBufferInternal[89] & InputLeft)
			&& InputBufferInternal[88] & InputUp && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case EInputCondition::Input_8:
		ResetInputSequence();
		InputSequence[0] = InputUp;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_8_Press:
		if (InputBufferInternal[89] & InputUp && !(InputBufferInternal[89] & InputLeft)
			&& !(InputBufferInternal[89] & InputRight) && !(InputBufferInternal[86] & InputUp))
		{
			return true;
		}
		break;
	case EInputCondition::Input_8_Hold:
		if (InputBufferInternal[89] & InputUp && !(InputBufferInternal[89] & InputLeft)
			&& !(InputBufferInternal[89] & InputRight) && InputBufferInternal[88] & InputUp)
		{
			return true;
		}
		break;
	case EInputCondition::Input_8_Release:
		if (!(InputBufferInternal[89] & InputUp) && InputBufferInternal[88] & InputUp)
		{
			return true;
		}
		break;
	case EInputCondition::Input_9:
		ResetInputSequence();
		InputSequence[0] = InputUpRight;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_9_Press:
		if (InputBufferInternal[89] & InputUp && InputBufferInternal[89] & InputRight
			&& !(InputBufferInternal[86] & InputUp && InputBufferInternal[86] & InputRight))
		{
			return true;
		}
		break;
	case EInputCondition::Input_9_Hold:
		if (InputBufferInternal[89] & InputUp && InputBufferInternal[89] & InputRight
			&& InputBufferInternal[88] & InputUp && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case EInputCondition::Input_9_Release:
		if (!(InputBufferInternal[89] & InputUp && InputBufferInternal[89] & InputRight)
			&& InputBufferInternal[88] & InputUp && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case EInputCondition::Input_Down:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		return CheckInputSequence();
	case EInputCondition::Input_Down_Press:
		if (InputBufferInternal[89] & InputDown && !(InputBufferInternal[86] & InputDown))
		{
			return true;
		}
		break;
	case EInputCondition::Input_Down_Hold:
		if (InputBufferInternal[89] & InputDown && InputBufferInternal[88] & InputDown)
		{
			return true;
		}
		break;
	case EInputCondition::Input_Down_Release:
		if (!(InputBufferInternal[89] & InputDown) && InputBufferInternal[88] & InputDown)
		{
			return true;
		}
		break;
	case EInputCondition::Input_Left:
		ResetInputSequence();
		InputSequence[0] = InputLeft;
		return CheckInputSequence();
	case EInputCondition::Input_Left_Press:
		if (InputBufferInternal[89] & InputLeft && !(InputBufferInternal[86] & InputLeft))
		{
			return true;
		}
		break;
	case EInputCondition::Input_Left_Hold:
		if (InputBufferInternal[89] & InputLeft && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case EInputCondition::Input_Left_Release:
		if (!(InputBufferInternal[89] & InputLeft) && InputBufferInternal[88] & InputLeft)
		{
			return true;
		}
		break;
	case EInputCondition::Input_Right:
		ResetInputSequence();
		InputSequence[0] = InputRight;
		return CheckInputSequence();
	case EInputCondition::Input_Right_Press:
		if (InputBufferInternal[89] & InputRight && !(InputBufferInternal[86] & InputRight))
		{
			return true;
		}
		break;
	case EInputCondition::Input_Right_Hold:
		if (InputBufferInternal[89] & InputRight && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case EInputCondition::Input_Right_Release:
		if (!(InputBufferInternal[89] & InputRight) && InputBufferInternal[88] & InputRight)
		{
			return true;
		}
		break;
	case EInputCondition::Input_Up:
		ResetInputSequence();
		InputSequence[0] = InputUp;
		return CheckInputSequence();
	case EInputCondition::Input_Up_Press:
		if (InputBufferInternal[89] & InputUp && !(InputBufferInternal[86] & InputUp))
		{
			return true;
		}
		break;
	case EInputCondition::Input_Up_Hold:
		if (InputBufferInternal[89] & InputUp && InputBufferInternal[88] & InputUp)
		{
			return true;
		}
		break;
	case EInputCondition::Input_Up_Release:
		if (!(InputBufferInternal[89] & InputUp) && InputBufferInternal[88] & InputUp)
		{
			return true;
		}
		break;
	case EInputCondition::Input_SuperJump:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		InputSequence[1] = InputUp;
		return CheckInputSequence();
	case EInputCondition::Input_SuperJump_Back:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		InputSequence[1] = InputUpLeft;
		return CheckInputSequence();
	case EInputCondition::Input_SuperJump_Forward:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		InputSequence[1] = InputUpRight;
		return CheckInputSequence();
	case EInputCondition::Input_DownDown:
		ResetInputSequence();
		InputSequence[0] = InputNeutral;
		InputSequence[1] = InputDown;
		InputSequence[2] = InputNeutral;
		InputSequence[3] = InputDown;
		return CheckInputSequence();
	case EInputCondition::Input_44:
		ResetInputSequence();
		InputSequence[0] = InputNeutral;
		InputSequence[1] = InputLeft;
		InputSequence[2] = InputNeutral;
		InputSequence[3] = InputLeft;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_66:
		ResetInputSequence();
		InputSequence[0] = InputNeutral;
		InputSequence[1] = InputRight;
		InputSequence[2] = InputNeutral;
		InputSequence[3] = InputRight;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_236:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		InputSequence[1] = InputRight;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_214:
		ResetInputSequence();
		InputSequence[0] = InputDown;
		InputSequence[1] = InputLeft;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_623:
		ResetInputSequence();
		InputSequence[0] = InputRight;
		InputSequence[1] = InputDown;
		InputSequence[2] = InputDownRight;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_421:
		ResetInputSequence();
		InputSequence[0] = InputLeft;
		InputSequence[1] = InputDown;
		InputSequence[2] = InputDownLeft;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_41236:
		ResetInputSequence();
		ImpreciseInputCount = 1;
		InputSequence[0] = InputLeft;
		InputSequence[1] = InputDown;
		InputSequence[2] = InputRight;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_63214:
		ResetInputSequence();
		ImpreciseInputCount = 1;
		InputSequence[0] = InputRight;
		InputSequence[1] = InputDown;
		InputSequence[2] = InputLeft;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_236236:
		ResetInputSequence();
		ImpreciseInputCount = 1;
		InputSequence[0] = InputDown;
		InputSequence[1] = InputRight;
		InputSequence[2] = InputDown;
		InputSequence[3] = InputRight;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_214214:
		ResetInputSequence();
		ImpreciseInputCount = 1;
		InputSequence[0] = InputDown;
		InputSequence[1] = InputLeft;
		InputSequence[2] = InputDown;
		InputSequence[3] = InputLeft;
		return CheckInputSequenceStrict();
	case EInputCondition::Input_L:
		ResetInputSequence();
		InputSequence[0] = InputL;
		Lenience = 3;
		return CheckInputSequence();
	case EInputCondition::Input_L_Press:
		ResetInputSequence();
		InputSequence[0] = InputL;
		Lenience = 3;
		return CheckInputSequenceOnce();
	case EInputCondition::Input_L_Hold:
		if (InputBufferInternal[89] & InputL && InputBufferInternal[88] & InputL)
		{
			return true;
		}
		break;
	case EInputCondition::Input_L_Release:
		if (!(InputBufferInternal[89] & InputL) && InputBufferInternal[88] & InputL)
		{
			return true;
		}
		break;
	case EInputCondition::Input_M:
		ResetInputSequence();
		InputSequence[0] = InputM;
		Lenience = 3;
		return CheckInputSequence();
	case EInputCondition::Input_M_Press:
		ResetInputSequence();
		InputSequence[0] = InputM;
		Lenience = 3;
		return CheckInputSequenceOnce();
	case EInputCondition::Input_M_Hold:
		if (InputBufferInternal[89] & InputM && InputBufferInternal[88] & InputM)
		{
			return true;
		}
		break;
	case EInputCondition::Input_M_Release:
		if (!(InputBufferInternal[89] & InputM) && InputBufferInternal[88] & InputM)
		{
			return true;
		}
		break;
	case EInputCondition::Input_H:
		ResetInputSequence();
		InputSequence[0] = InputH;
		Lenience = 3;
		return CheckInputSequence();
	case EInputCondition::Input_H_Press:
		ResetInputSequence();
		InputSequence[0] = InputH;
		Lenience = 3;
		return CheckInputSequenceOnce();
	case EInputCondition::Input_H_Hold:
		if (InputBufferInternal[89] & InputH && InputBufferInternal[88] & InputH)
		{
			return true;
		}
		break;
	case EInputCondition::Input_H_Release:
		if (!(InputBufferInternal[89] & InputH) && InputBufferInternal[88] & InputH)
		{
			return true;
		}
		break;
	case EInputCondition::Input_S:
		ResetInputSequence();
		InputSequence[0] = InputS;
		Lenience = 3;
		return CheckInputSequence();
	case EInputCondition::Input_S_Press:
		ResetInputSequence();
		InputSequence[0] = InputS;
		Lenience = 3;
		return CheckInputSequenceOnce();
	case EInputCondition::Input_S_Hold:
		if (InputBufferInternal[89] & InputS && InputBufferInternal[88] & InputS)
		{
			return true;
		}
		break;
	case EInputCondition::Input_S_Release:
		if (!(InputBufferInternal[89] & InputS) && InputBufferInternal[88] & InputS)
		{
			return true;
		}
	case EInputCondition::Input_L_And_S:
		ResetInputSequence();
		InputSequence[0] = InputL;
		InputSequence[1] = InputS;
		Lenience = 3;
		return CheckInputSequenceOnce();
	default:
		return false;
	}
	return false;
}

bool FInputBuffer::CheckInputSequence()
{
	int32 InputIndex = -10;
	for (int32 i = 19; i > -1; i--)
	{
		if (InputSequence[i] != -1)
		{
			InputIndex = i;
			break;
		}
	}
	int32 FramesSinceLastMatch = 0; //how long it's been since last input match
	bool NoMatches = true;

	for (int32 i = 89; i >= 0; i--)
	{
		if (InputIndex == -1) //check if input sequence has been fully read
			return true;
		
		if (bIsFinalSequence && NoMatches && InputDisabled[i])
			return false;
		
		const int32 NeededInput = InputSequence[InputIndex];
		if (FramesSinceLastMatch > Lenience)
			return false;
		FramesSinceLastMatch++;

		if ((InputBufferInternal[i] & NeededInput) == NeededInput) //if input matches...
		{
			NoMatches = false;
			InputIndex--; //advance sequence
			FramesSinceLastMatch = 0; //reset last match
			i--;
		}
	}

	return false;
}

bool FInputBuffer::CheckInputSequenceStrict()
{
	int32 InputIndex = -10;
	for (int32 i = 19; i > -1; i--)
	{
		if (InputSequence[i] != -1)
		{
			InputIndex = i;
			break;
		}
	}
	int32 FramesSinceLastMatch = 0; //how long it's been since last input match
	int32 ImpreciseMatches = 0;
	bool NoMatches = true;
	
	for (int32 i = 89; i >= 0; i--)
	{
		if (InputIndex == -1) //check if input sequence has been fully read
			return true;

		if (bIsFinalSequence && NoMatches && InputDisabled[i])
			return false;
		
		const int32 NeededInput = InputSequence[InputIndex];
		if (FramesSinceLastMatch > Lenience)
			return false;
		FramesSinceLastMatch++;

		if ((InputBufferInternal[i] ^ NeededInput) << 26 == 0) //if input matches...
		{
			NoMatches = false;
			InputIndex--; //advance sequence
			FramesSinceLastMatch = 0; //reset last match
			i--;
			continue;
		}
		if ((InputBufferInternal[i] & NeededInput) == NeededInput) //if input matches...
		{
			NoMatches = false;
			if (ImpreciseMatches >= ImpreciseInputCount)
				continue;
			ImpreciseMatches++;
			InputIndex--; //advance sequence
			FramesSinceLastMatch = 0; //reset last match
			i--;
		}
	}

	return false;
}

bool FInputBuffer::CheckInputSequenceOnce()
{
	int32 InputIndex = -10;
	for (int32 i = 19; i > -1; i--)
	{
		if (InputSequence[i] != -1)
		{
			InputIndex = i;
			break;
		}
	}
	int32 FramesSinceLastMatch = 0; //how long it's been since last input match
	bool NoMatches = true;

	for (int32 i = 89; i >= 0; i--)
	{
		if (InputIndex < 0) //check if input sequence has been fully read
		{
			if (InputIndex <= -Lenience)
				return false;
			if (!(InputBufferInternal[i] & InputSequence[0]))
				return true;
			InputIndex--;
			continue;
		}
		const int32 NeededInput = InputSequence[InputIndex];

		if (bIsFinalSequence && NoMatches && InputDisabled[i])
			return false;

		if (FramesSinceLastMatch > Lenience + 3)
			return false;
		FramesSinceLastMatch++;

		if ((InputBufferInternal[i] & NeededInput) == NeededInput) //if input matches...
		{
			NoMatches = false;
			InputIndex--; //advance sequence
			FramesSinceLastMatch = 0; //reset last match
			i--;
		}
	}

	return false;
}

void FInputBuffer::ResetInputSequence()
{
	for (int32 i = 0; i < 20; i++)
	{
		InputSequence[i] = -1;
	}
}

void FInputBuffer::FlipInputsInBuffer()
{
	for (int i = 0; i < 90; i++)
	{
		const unsigned int Bit1 = (InputBufferInternal[i] >> 2) & 1;
		const unsigned int Bit2 = (InputBufferInternal[i] >> 3) & 1;
		unsigned int x = (Bit1 ^ Bit2);

		x = x << 2 | x << 3;

		InputBufferInternal[i] = InputBufferInternal[i] ^ x;
	}
}
