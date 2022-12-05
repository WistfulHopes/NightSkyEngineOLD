// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintSubroutine.h"

BlueprintSubroutine::BlueprintSubroutine(USubroutine* InSubroutine)
{
	Owner = InSubroutine;
}

void BlueprintSubroutine::OnCall()
{
	Owner->OnCall();
}
