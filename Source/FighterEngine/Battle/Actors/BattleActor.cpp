// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleActor.h"

#include "CanvasItem.h"
#include "FighterGameState.h"
#include "NiagaraComponent.h"
#include "PlayerCharacter.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "../State.h"

// Sets default values
ABattleActor::ABattleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;
}

// Called when the game starts or when spawned
void ABattleActor::BeginPlay()
{
	Super::BeginPlay();
	if (IsPlayer)
	{
		Player = Cast<APlayerCharacter>(this);
	}
}

// Called every frame
void ABattleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (LinkedParticle != nullptr)
	{
		FVector Location = FVector(0, PosX / COORD_SCALE, PosY / COORD_SCALE);
		LinkedParticle->SetWorldLocation(Location);
	}
	if (IsActive)
	{
		if (LinkedParticle != nullptr)
		{
			LinkedParticle->SetVisibility(true);
		}
	}
}

void ABattleActor::InitObject()
{
	ObjectState->ObjectParent = this;
	ObjectState->Parent = Player;
	SetActorLocation(FVector(0, float(PosX) / COORD_SCALE, float(PosY) / COORD_SCALE)); //set visual location and scale in unreal
	if (!FacingRight)
	{
		SetActorScale3D(FVector(1, -1, 1));
	}
	else
	{
		SetActorScale3D(FVector(1, 1, 1));
	}
	if (LinkedParticle != nullptr)
	{
		LinkedParticle->SetWorldLocation(GetActorLocation());
	}
	Hitstop = 0;
	if (PosY == 0 && PrevPosY != 0)
		Gravity = 1900;
	ObjectState->OnEnter();
}

void ABattleActor::Update()
{
	if (DeactivateOnNextUpdate)
	{
		ResetObject();
		return;
	}
	if (Hitstop > 0) //break if hitstop active. internal hitstop values are 1 greater than set by blueprints as a shoddy bug fix.
	{
		Hitstop--;
		return;
	}
	Hitstop--;
	GetBoxes(); //get boxes from cel name
	Move(); //handle movement
	AnimTime++; //increments counters
	AnimBPTime++;
	ActionTime++;
	if (IsPlayer) //initializes player only values
	{
		if (Player->ActionFlags == ACT_Standing) //set pushbox values based on state
		{
			PushWidth = Player->StandPushWidth;
			PushHeight = Player->StandPushHeight;
			PushHeightLow = 0;
		}
		else if (Player->ActionFlags == ACT_Crouching)
		{
			PushWidth = Player->CrouchPushWidth;
			PushHeight = Player->CrouchPushHeight;
			PushHeightLow = 0;
		}
		else if (Player->ActionFlags == ACT_Jumping)
		{
			PushWidth = Player->AirPushWidth;
			PushHeight = Player->AirPushHeight;
			PushHeightLow = Player->AirPushHeightLow;
		}
		if (RoundStart)
		{
			if (Player->PlayerIndex == 0)
			{
				FacingRight = true;
				PosX = -350000;
			}
			else
			{
				FacingRight = false;
				PosX = 350000;
			}
		}
	}
	L = PosX - PushWidth / 2; //sets pushboxes
	R = PosX + PushWidth / 2;
	T = PosY + PushHeight;
	B = PosY - PushHeightLow;
	if (!IsPlayer)
	{
		ObjectState->OnUpdate(1/60);
	}
	RoundStart = false; //round has started
}

void ABattleActor::Move()
{
	PrevPosX = PosX; //set previous pos values
	PrevPosY = PosY;
	if (IsPlayer && Player != nullptr)
	{
		if (Player->AirDashTimer <= 0) //only set gravity if air dash timer isn't active
		{
			AddPosY(SpeedY);
			SpeedY -= Gravity;
		}
	}
	else
	{
		AddPosY(SpeedY);
		SpeedY -= Gravity; //set gravity
	}
	AddPosX(SpeedX); //apply speed
	if (MiscFlags & MISC_InertiaEnable) //only use inertia if enabled
	{
		if (PosY <= 0) //only decrease inertia if grounded
		{
			Inertia = Inertia - Inertia / 10;
		}
		if (Inertia > -875 && Inertia < 875) //if inertia small enough, set to zero
		{
			ClearInertia();
		}
		AddPosX(Inertia);
	}
	if (PosY < 0) //if on ground, force y values to zero
	{
		PosY = 0;
		SpeedY = 0;
	}
	SetActorLocation(FVector(0, float(PosX) / COORD_SCALE, float(PosY) / COORD_SCALE)); //set visual location and scale in unreal
	if (!FacingRight)
	{
		SetActorScale3D(FVector(1, -1, 1));
	}
	else
	{
		SetActorScale3D(FVector(1, 1, 1));
	}
}

void ABattleActor::SetPosX(int InPosX)
{
	PosX = InPosX;
}

void ABattleActor::SetPosY(int InPosY)
{
	PosY = InPosY;
}

void ABattleActor::AddPosX(int InPosX)
{
	if (FacingRight)
	{
		PosX += InPosX;
	}
	else
	{
		PosX -= InPosX;
	}
}

void ABattleActor::AddPosXRaw(int InPosX)
{
	PosX += InPosX;
}

void ABattleActor::AddPosY(int InPosY)
{
	PosY += InPosY;
}

void ABattleActor::SetSpeedX(int InSpeedX)
{
	SpeedX = InSpeedX;
}

void ABattleActor::SetSpeedY(int InSpeedY)
{
	SpeedY = InSpeedY;
}

void ABattleActor::SetGravity(int InGravity)
{
	Gravity = InGravity;
}

void ABattleActor::HaltMomentum()
{
	SpeedX = 0;
	SpeedY = 0;
	ClearInertia();
}

int ABattleActor::GetInternalValue(EInternalValue InternalValue)
{
	switch (InternalValue)
	{
	case VAL_ActionFlag:
		if (IsPlayer && Player != nullptr) //only available as player character
		{
			return Player->ActionFlags;
		};
		break;
	case VAL_PosX:
		return PosX;
	case VAL_PosY:
		return PosY;
	case VAL_SpeedX:
		return SpeedX;
	case VAL_SpeedY:
		return SpeedY;
	case VAL_ActionTime:
		return ActionTime;
	case VAL_Inertia:
		return Inertia;
	case VAL_FacingRight:
		return FacingRight;
	case VAL_Hitstop:
		return Hitstop;
	case VAL_Health:
		if (IsPlayer && Player != nullptr) //only available as player character
		{
			return Player->Health;
		};
		break;
	default:
		return 0;
	}
	return 0;
}

bool ABattleActor::IsOnFrame(int Frame)
{
	if (Frame == AnimTime)
	{
		return true;
	}
	return false;
}

void ABattleActor::SetCelName(FString InCelName)
{
	CelName = InCelName;
	CelNameInternal.SetString(InCelName);
}

void ABattleActor::SetHitEffectName(FString InHitEffectName)
{
	HitEffectName.SetString(InHitEffectName);
}

void ABattleActor::AddSpeedX(int InSpeedX)
{
	SpeedX += InSpeedX;
}

void ABattleActor::AddSpeedY(int InSpeedY)
{
	SpeedY += InSpeedY;
}

void ABattleActor::SetInertia(int InInertia)
{
	Inertia = InInertia;
}

void ABattleActor::ClearInertia()
{
	Inertia = 0;
}

void ABattleActor::EnableInertia()
{
	MiscFlags |= MISC_InertiaEnable;
}

void ABattleActor::DisableInertia()
{
	MiscFlags = MiscFlags & ~MISC_InertiaEnable;
}

void ABattleActor::HandlePushCollision(ABattleActor* OtherObj)
{
	if (Hitstop <= 0)
	{
		if (T >= OtherObj->B && B <= OtherObj->T && R >= OtherObj->L && L <= OtherObj->R)
		{
			bool IsPushLeft;
			int CollisionDepth;
			int PosXOffset;
			if(PosX == OtherObj->PosX)
			{
				IsPushLeft = PrevPosX < OtherObj->PrevPosX;
			}
			else
			{
				IsPushLeft = PosX < OtherObj->PosX;
			}
			if (IsPushLeft)
			{
				CollisionDepth = OtherObj->L - R;
			}
			else
			{
				CollisionDepth = OtherObj->R - L;
			}
			PosXOffset = CollisionDepth / 2;
			AddPosXRaw(PosXOffset);
		}
	}
}

void ABattleActor::SetFacing(bool NewFacingRight)
{
	FacingRight = NewFacingRight;
}

void ABattleActor::EnableFlip(bool Enabled)
{
	if (Enabled)
	{
		MiscFlags |= MISC_FlipEnable;
	}
	else
	{
		MiscFlags = MiscFlags & ~MISC_FlipEnable;
	}
}

void ABattleActor::GetBoxes()
{
	CollisionBoxes.Empty();
	if (Player->CollisionData != nullptr)
	{
		for (int i = 0; i < Player->CollisionData->CollisionFrames.Num(); i++)
		{
			if (Player->CollisionData->CollisionFrames[i].Name == CelNameInternal.GetString())
			{
				for (int j = 0; j < 0x20; j++)
				{
					if (j < Player->CollisionData->CollisionFrames[i].CollisionBoxes.Num())
					{
						FCollisionBoxInternal CollisionBoxInternal;
						CollisionBoxInternal.Type = Player->CollisionData->CollisionFrames[i].CollisionBoxes[j]->Type;
						CollisionBoxInternal.PosX = Player->CollisionData->CollisionFrames[i].CollisionBoxes[j]->PosX;
						CollisionBoxInternal.PosY = Player->CollisionData->CollisionFrames[i].CollisionBoxes[j]->PosY;
						CollisionBoxInternal.SizeX = Player->CollisionData->CollisionFrames[i].CollisionBoxes[j]->SizeX;
						CollisionBoxInternal.SizeY = Player->CollisionData->CollisionFrames[i].CollisionBoxes[j]->SizeY;
						CollisionBoxes.Add(CollisionBoxInternal);
						CollisionBoxesInternal[j] = CollisionBoxInternal;
					}
					else
					{
						CollisionBoxesInternal[j].Type = Hurtbox;
						CollisionBoxesInternal[j].PosX = 0;
						CollisionBoxesInternal[j].PosY = 0;
						CollisionBoxesInternal[j].SizeX = 0;
						CollisionBoxesInternal[j].SizeY = 0;
					}
				}
			}
		}
	}
}

//for collision viewer

template<typename T>
constexpr auto min(T a, T b)
{
	return a < b ? a : b;
}

template<typename T>
constexpr auto max(T a, T b)
{
	return a > b ? a : b;
}
static void clip_line_y(
	const FVector2D &line_a, const FVector2D &line_b,
	float min_x, float max_x,
	float *min_y, float *max_y)
{
	const auto delta = line_b - line_a;

	if (abs(delta.X) > FLT_EPSILON) {
		const auto slope = delta.Y / delta.X;
		const auto intercept = line_a.Y - slope * line_a.X;
		*min_y = slope * min_x + intercept;
		*max_y = slope * max_x + intercept;
	} else {
		*min_y = line_a.Y;
		*max_y = line_b.Y;
	}

	if (*min_y > *max_y)
		std::swap(*min_y, *max_y);
}

bool line_box_intersection(
	const FVector2D &box_min, const FVector2D &box_max,
	const FVector2D &line_a, const FVector2D &line_b,
	float *entry_fraction, float *exit_fraction)
{
	// No intersection if line runs along the edge of the box
	if (line_a.X == line_b.X && (line_a.X == box_min.X || line_a.X == box_max.X))
		return false;

	if (line_a.Y == line_b.Y && (line_a.Y == box_min.Y || line_a.Y == box_max.Y))
		return false;

	// Clip X values to segment within box_min.X and box_max.X
	const auto min_x = max(min(line_a.X, line_b.X), box_min.X);
	const auto max_x = min(max(line_a.X, line_b.X), box_max.X);

	// Check if the line is in the bounds of the box on the X axis
	if (min_x > max_x)
		return false;

	// Clip Y values to segment within min_x and max_x
	float min_y, max_y;
	clip_line_y(line_a, line_b, min_x, max_x, &min_y, &max_y);

	// Clip Y values to segment within box_min.Y and box_max.Y
	min_y = max(min_y, (float)box_min.Y);
	max_y = min(max_y, (float)box_max.Y);

	// Check if the clipped line is in the bounds of the box on the Y axis
	if (min_y > max_y)
		return false;

	const FVector2D entry(
		line_a.X < line_b.X ? min_x : max_x,
		line_a.Y < line_b.Y ? min_y : max_y);

	const FVector2D exit(
		line_a.X > line_b.X ? min_x : max_x,
		line_a.Y > line_b.Y ? min_y : max_y);

	const auto length = (line_b - line_a).Size();
	*entry_fraction = (entry - line_a).Size() / length;
	*exit_fraction = (exit - line_a).Size() / length;

	return true;
}

void ABattleActor::CollisionView()
{
	TArray<TArray<FVector2D>> Corners;
	TArray<TArray<TArray<FVector2D>>> Lines; 
	if (CollisionBoxes.Num() != 0)
	{
		for (int i = 0; i < CollisionBoxes.Num(); i++)
		{
			TArray<FVector2D> CurrentCorners;
			if (FacingRight)
			{
				CurrentCorners.Add(FVector2D(float(CollisionBoxes[i].PosX + PosX) / COORD_SCALE - float(CollisionBoxes[i].SizeX) / COORD_SCALE / 2,
					float(CollisionBoxes[i].PosY + PosY) / COORD_SCALE -  float(CollisionBoxes[i].SizeY) / COORD_SCALE / 2));
				CurrentCorners.Add(FVector2D(float(CollisionBoxes[i].PosX + PosX) / COORD_SCALE + float(CollisionBoxes[i].SizeX) / COORD_SCALE / 2,
					float(CollisionBoxes[i].PosY + PosY) / COORD_SCALE -  float(CollisionBoxes[i].SizeY) / COORD_SCALE / 2));
				CurrentCorners.Add(FVector2D(float(CollisionBoxes[i].PosX + PosX) / COORD_SCALE + float(CollisionBoxes[i].SizeX) / COORD_SCALE / 2,
					float(CollisionBoxes[i].PosY + PosY) / COORD_SCALE + float(CollisionBoxes[i].SizeY) / COORD_SCALE / 2));
				CurrentCorners.Add(FVector2D(float(CollisionBoxes[i].PosX + PosX) / COORD_SCALE - float(CollisionBoxes[i].SizeX) / COORD_SCALE / 2,
					float(CollisionBoxes[i].PosY + PosY) / COORD_SCALE + float(CollisionBoxes[i].SizeY) / COORD_SCALE / 2));
			}
			else
			{
				CurrentCorners.Add(FVector2D(float(-CollisionBoxes[i].PosX + PosX) / COORD_SCALE - float(CollisionBoxes[i].SizeX) / COORD_SCALE / 2,
					float(CollisionBoxes[i].PosY + PosY) / COORD_SCALE -  float(CollisionBoxes[i].SizeY) / COORD_SCALE / 2));
				CurrentCorners.Add(FVector2D(float(-CollisionBoxes[i].PosX + PosX) / COORD_SCALE + float(CollisionBoxes[i].SizeX) / COORD_SCALE / 2,
					float(CollisionBoxes[i].PosY + PosY) / COORD_SCALE -  float(CollisionBoxes[i].SizeY) / COORD_SCALE / 2));
				CurrentCorners.Add(FVector2D(float(-CollisionBoxes[i].PosX + PosX) / COORD_SCALE + float(CollisionBoxes[i].SizeX) / COORD_SCALE / 2,
					float(CollisionBoxes[i].PosY + PosY) / COORD_SCALE + float(CollisionBoxes[i].SizeY) / COORD_SCALE / 2));
				CurrentCorners.Add(FVector2D(float(-CollisionBoxes[i].PosX + PosX) / COORD_SCALE - float(CollisionBoxes[i].SizeX) / COORD_SCALE / 2,
					float(CollisionBoxes[i].PosY + PosY) / COORD_SCALE + float(CollisionBoxes[i].SizeY) / COORD_SCALE / 2));
			}
			Corners.Add(CurrentCorners);
			TArray<TArray<FVector2D>> CurrentLines;
			for (int j = 0; j < 4; j++)
			{
				CurrentLines.Add(TArray { CurrentCorners[j] , CurrentCorners[(j + 1) % 4] } );
			}
			Lines.Add(CurrentLines);
		}
		for (int i = 0; i < CollisionBoxes.Num(); i++)
		{
			FLinearColor color;
			if (CollisionBoxes[i].Type == Hitbox)
				color = FLinearColor(1.f, 0.f, 0.f, .25f);
			else if (IsAttacking)
				color = FLinearColor(0.f, 1.f, 1.f, .25f);
			else
				color = FLinearColor(0.f, 1.f, 0.f, .25f);

			for (const auto LineSet : Lines[i])
			{
				auto start = LineSet[0];
				auto end = LineSet[1];
				DrawDebugLine(GetWorld(), FVector(0, start.X, start.Y), FVector(0, end.X, end.Y), color.ToFColor(false), false, 1 / 60, 255, 2.f);
			}
		}
	}
	TArray<FVector2D> CurrentCorners;
	CurrentCorners.Add(FVector2D(L / COORD_SCALE, B / COORD_SCALE));
	CurrentCorners.Add(FVector2D(R / COORD_SCALE, B / COORD_SCALE));
	CurrentCorners.Add(FVector2D(R / COORD_SCALE, T / COORD_SCALE));
	CurrentCorners.Add(FVector2D(L / COORD_SCALE, T / COORD_SCALE));
	TArray<TArray<FVector2D>> CurrentLines;
	for (int j = 0; j < 4; j++)
	{
		CurrentLines.Add(TArray { CurrentCorners[j] , CurrentCorners[(j + 1) % 4] } );
	}
	FLinearColor color = FLinearColor(1.f, 1.f, 0.f, .2f);

	for (const auto LineSet : CurrentLines)
	{
		auto start = LineSet[0];
		auto end = LineSet[1];
		DrawDebugLine(GetWorld(), FVector(0, start.X, start.Y), FVector(0, end.X, end.Y), color.ToFColor(false), false,1 / 60, 255, 2.f);
	}
}

//collision viewer end

void ABattleActor::HandleHitCollision(APlayerCharacter* OtherChar)
{
	if (HitActive && !OtherChar->StrikeInvulnerable && OtherChar != Player)
	{
		for (int i = 0; i < 0x20; i++)
		{
			if (CollisionBoxesInternal[i].Type == Hitbox)
			{
				for (int j = 0; j < 0x20; j++)
				{
					if (OtherChar->CollisionBoxesInternal[j].Type == Hurtbox)
					{
						FCollisionBoxInternal Hitbox = CollisionBoxesInternal[i];

						FCollisionBoxInternal Hurtbox = OtherChar->CollisionBoxesInternal[j];

						if (FacingRight)
						{
							Hitbox.PosX += PosX;
						}
						else
						{
							Hitbox.PosX = -Hitbox.PosX + PosX;  
						}
						Hitbox.PosY += PosY;
						if (OtherChar->FacingRight)
						{
							Hurtbox.PosX += OtherChar->PosX;
						}
						else
						{
							Hurtbox.PosX = -Hurtbox.PosX + OtherChar->PosX;  
						}
						Hurtbox.PosY += OtherChar->PosY;
						
						if (Hitbox.PosY + Hitbox.SizeY / 2 >= Hurtbox.PosY - Hurtbox.SizeY / 2
							&& Hitbox.PosY - Hitbox.SizeY / 2 <= Hurtbox.PosY + Hurtbox.SizeY / 2
							&& Hitbox.PosX + Hitbox.SizeX / 2 >= Hurtbox.PosX - Hurtbox.SizeX / 2
							&& Hitbox.PosX - Hitbox.SizeX / 2 <= Hurtbox.PosX + Hurtbox.SizeX / 2)
						{
							HitActive = false;
							HasHit = true;
							int CollisionDepthX;
							if (Hitbox.PosX < Hurtbox.PosX)
								CollisionDepthX = Hurtbox.PosX - Hurtbox.SizeX / 2 - Hitbox.PosX - Hitbox.SizeX / 2;
							else
								CollisionDepthX = Hurtbox.PosX + Hurtbox.SizeX / 2 - Hitbox.PosX + Hitbox.SizeX / 2;
							int CollisionDepthY;
							if (Hitbox.PosY < Hurtbox.PosY)
								CollisionDepthY = Hurtbox.PosY - Hurtbox.SizeY / 2 - Hitbox.PosY - Hitbox.SizeY / 2;
							else
								CollisionDepthY = Hurtbox.PosY + Hurtbox.SizeY / 2 - Hitbox.PosY + Hitbox.SizeY / 2;
							HitPosX = Hitbox.PosX - CollisionDepthX / 2;
							HitPosY = Hitbox.PosY - CollisionDepthY / 2;

							if (OtherChar->EnableFlags & ENB_Block || OtherChar->Blockstun > 0) //check blocking
							{
								if (OtherChar->IsCorrectBlock(HitEffect.BlockType))
								{
									OtherChar->Hitstop = HitEffect.Hitstop;
									OtherChar->Blockstun = HitEffect.Blockstun;
									Hitstop = HitEffect.Hitstop;
									if (OtherChar->PosY)
									{
										if (!OtherChar->TouchingWall)
										{
											OtherChar->SetInertia(-HitEffect.HitPushbackX);
										}
										else
										{
											if (IsPlayer && Player != nullptr)
											{
												SetInertia(-HitEffect.HitPushbackX);
											}
										}
									}
									else
									{
										OtherChar->ClearInertia();
										if (!OtherChar->TouchingWall)
										{
											OtherChar->SetInertia(-HitEffect.AirHitPushbackX);
										}
										else
										{
											if (IsPlayer && Player != nullptr)
											{
												SetInertia(-HitEffect.AirHitPushbackX);
											}
										}
										OtherChar->SetSpeedY(HitEffect.AirHitPushbackY);
										OtherChar->AirDashTimer = 0;
									}
									OtherChar->HandleBlockAction();
									return;
								}
							}
							if (!OtherChar->IsAttacking)
							{
								if (IsPlayer)
									Player->StateMachine->GetCurrentState()->OnHit();
								else
									ObjectState->OnHit();
								
								int32 Proration = HitEffect.ForcedProration;
								if (Player->ComboCounter == 0)
									Proration *= HitEffect.InitialProration;
								else
									Proration *= 100;
								if (Player->ComboCounter == 0)
									OtherChar->TotalProration = 10000;
								Proration = Proration * OtherChar->TotalProration / 10000;
								OtherChar->TotalProration = OtherChar->TotalProration * HitEffect.ForcedProration / 100;
								int FinalDamage;
								if (Player->ComboCounter == 0)
									FinalDamage = HitEffect.HitDamage;
								else
									FinalDamage = HitEffect.HitDamage * Proration * Player->ComboRate / 1000000;
								
								OtherChar->CurrentHealth -= FinalDamage;
								Player->ComboCounter++;
								if (OtherChar->CurrentHealth < 0)
									OtherChar->CurrentHealth = 0;
								OtherChar->Hitstun = HitEffect.Hitstun;
								OtherChar->Untech = HitEffect.Untech;
								OtherChar->Hitstop = HitEffect.Hitstop;
								Hitstop = HitEffect.Hitstop;
								OtherChar->Gravity = HitEffect.HitGravity;
								if (OtherChar->PosY == 0)
								{
									if (HitEffect.GroundHitAction == HACT_GroundNormal)
									{
										if (!OtherChar->TouchingWall)
										{
											OtherChar->SetInertia(-HitEffect.HitPushbackX);
										}
										else
										{
											if (IsPlayer && Player != nullptr)
											{
												SetInertia(-HitEffect.HitPushbackX);
											}
										}
									}
									else if (HitEffect.GroundHitAction == HACT_AirNormal)
									{
										OtherChar->ClearInertia();
										if (!OtherChar->TouchingWall)
										{
											OtherChar->SetInertia(-HitEffect.AirHitPushbackX);
										}
										else
										{
											if (IsPlayer && Player != nullptr)
											{
												SetInertia(-HitEffect.AirHitPushbackX);
											}
										}
										OtherChar->SetSpeedY(HitEffect.AirHitPushbackY);
									}
									OtherChar->HandleHitAction(HitEffect.GroundHitAction, HitEffect.AttackLevel);
								}
								else
								{
									if (HitEffect.AirHitAction == HACT_GroundNormal)
									{
										if (!OtherChar->TouchingWall)
										{
											OtherChar->SetInertia(-HitEffect.HitPushbackX);
										}
										else
										{
											if (IsPlayer && Player != nullptr)
											{
												SetInertia(-HitEffect.HitPushbackX);
											}
										}
									}
									else if (HitEffect.AirHitAction == HACT_AirNormal)
									{
										OtherChar->ClearInertia();
										if (!OtherChar->TouchingWall)
										{
											OtherChar->SetInertia(-HitEffect.AirHitPushbackX);
										}
										else
										{
											if (IsPlayer && Player != nullptr)
											{
												SetInertia(-HitEffect.AirHitPushbackX);
											}
										}
										OtherChar->SetSpeedY(HitEffect.AirHitPushbackY);
									}
									OtherChar->HandleHitAction(HitEffect.AirHitAction, HitEffect.AttackLevel);
									OtherChar->AirDashTimer = 0;
								}
								if (strcmp(HitEffectName.GetString(), ""))
								{
									CreateCharaParticle(FString(HitEffectName.GetString()), POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
									if (HitEffect.AttackLevel < 1)
									{
										PlayCommonSound("HitMeleeS");
									}
									else if (HitEffect.AttackLevel < 3)
									{
										PlayCommonSound("HitMeleeM");
									}
									else
									{
										PlayCommonSound("HitMeleeL");
									}
								}
								else if (ObjectState != nullptr)
								{
								    if (ObjectState->StateType == EStateType::SpecialAttack)
								    {
										CreateCommonParticle("cmn_hit_sp", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
                                        if (HitEffect.AttackLevel < 1)
                                        {
                                            PlayCommonSound("HitMeleeS");
                                        }
                                        else if (HitEffect.AttackLevel < 3)
                                        {
                                            PlayCommonSound("HitMeleeM");
                                        }
                                        else
                                        {
                                            PlayCommonSound("HitMeleeL");
                                        }								    
								    }
								    else
								    {
								    	if (HitEffect.AttackLevel < 1)
								    	{
								    		CreateCommonParticle("cmn_hit_s", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeS");
								    	}
								    	else if (HitEffect.AttackLevel < 3)
								    	{
								    		CreateCommonParticle("cmn_hit_m", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeM");
								    	}
								    	else
								    	{
								    		CreateCommonParticle("cmn_hit_l", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeL");
								    	}
								    }
								}
								else if (IsPlayer)
								{
								    if (Player->StateMachine->GetCurrentState()->StateType == EStateType::SpecialAttack)
								    {
										CreateCommonParticle("cmn_hit_sp", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
                                        if (HitEffect.AttackLevel < 1)
                                        {
                                            PlayCommonSound("HitMeleeS");
                                        }
                                        else if (HitEffect.AttackLevel < 3)
                                        {
                                            PlayCommonSound("HitMeleeM");
                                        }
                                        else
                                        {
                                            PlayCommonSound("HitMeleeL");
                                        }								    
								    }
								    else
								    {
								    	if (HitEffect.AttackLevel < 1)
								    	{
								    		CreateCommonParticle("cmn_hit_s", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeS");
								    	}
								    	else if (HitEffect.AttackLevel < 3)
								    	{
								    		CreateCommonParticle("cmn_hit_m", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeM");
								    	}
								    	else
								    	{
								    		CreateCommonParticle("cmn_hit_l", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeL");
								    	}
								    }
								}
							}
							else
							{
								if (IsPlayer)
                            	    Player->StateMachine->GetCurrentState()->OnCounterHit();
                            	else
                            	    ObjectState->OnCounterHit();

								int32 Proration = CounterHitEffect.ForcedProration;
								if (Player->ComboCounter == 0)
									Proration *= CounterHitEffect.InitialProration;
								else
									Proration *= 100;
								if (Player->ComboCounter == 0)
									OtherChar->TotalProration = 10000;
								Proration = Proration * OtherChar->TotalProration / 10000;
								OtherChar->TotalProration = OtherChar->TotalProration * CounterHitEffect.ForcedProration / 100;
								int FinalDamage;
								if (Player->ComboCounter == 0)
									FinalDamage = CounterHitEffect.HitDamage;
								else
									FinalDamage = CounterHitEffect.HitDamage * Proration * Player->ComboRate / 1000000;

								OtherChar->CurrentHealth -= FinalDamage;
								Player->ComboCounter++;
								if (OtherChar->CurrentHealth < 0)
									OtherChar->CurrentHealth = 0;
								OtherChar->Hitstun = CounterHitEffect.Hitstun;
								OtherChar->Untech = CounterHitEffect.Untech;
								OtherChar->Hitstop = CounterHitEffect.Hitstop;
								Hitstop = HitEffect.Hitstop;
								OtherChar->Gravity = CounterHitEffect.HitGravity;
								if (OtherChar->PosY == 0)
								{
									if (CounterHitEffect.GroundHitAction == HACT_GroundNormal)
									{
										if (!OtherChar->TouchingWall)
										{
											OtherChar->SetInertia(-CounterHitEffect.HitPushbackX);
										}
										else
										{
											if (IsPlayer && Player != nullptr)
											{
												SetInertia(-CounterHitEffect.HitPushbackX);
											}
										}
									}
									else if (CounterHitEffect.GroundHitAction == HACT_AirNormal)
									{
										OtherChar->ClearInertia();
										if (!OtherChar->TouchingWall)
										{
											OtherChar->SetInertia(-CounterHitEffect.AirHitPushbackX);
										}
										else
										{
											if (IsPlayer && Player != nullptr)
											{
												SetInertia(-CounterHitEffect.AirHitPushbackX);
											}
										}
										OtherChar->SetSpeedY(CounterHitEffect.AirHitPushbackY);
									}
									OtherChar->HandleHitAction(CounterHitEffect.GroundHitAction, CounterHitEffect.AttackLevel);
								}
								else
								{
									if (CounterHitEffect.AirHitAction == HACT_GroundNormal)
									{
										if (!OtherChar->TouchingWall)
										{
											OtherChar->SetInertia(-CounterHitEffect.HitPushbackX);
										}
										else
										{
											if (IsPlayer && Player != nullptr)
											{
												SetInertia(-CounterHitEffect.HitPushbackX);
											}
										}
									}
									else if (CounterHitEffect.AirHitAction == HACT_AirNormal)
									{
										OtherChar->ClearInertia();
										if (!OtherChar->TouchingWall)
										{
											OtherChar->SetInertia(-CounterHitEffect.AirHitPushbackX);
										}
										else
										{
											if (IsPlayer && Player != nullptr)
											{
												SetInertia(-CounterHitEffect.AirHitPushbackX);
											}
										}
										OtherChar->SetSpeedY(CounterHitEffect.AirHitPushbackY);
									}
									OtherChar->HandleHitAction(CounterHitEffect.AirHitAction, CounterHitEffect.AttackLevel);
									OtherChar->AirDashTimer = 0;
								}
								if (strcmp(HitEffectName.GetString(), ""))
								{
									CreateCharaParticle(FString(HitEffectName.GetString()), POS_Hit, FVector(-50, 0, 0), FRotator(-CounterHitEffect.HitAngle, 0, 0));
									if (HitEffect.AttackLevel < 1)
									{
										PlayCommonSound("HitMeleeS");
									}
									else if (HitEffect.AttackLevel < 3)
									{
										PlayCommonSound("HitMeleeM");
									}
									else
									{
										PlayCommonSound("HitMeleeL");
									}
								}
								else if (ObjectState != nullptr)
								{
								    if (ObjectState->StateType == EStateType::SpecialAttack)
								    {
										CreateCommonParticle("cmn_hit_sp", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
                                        if (HitEffect.AttackLevel < 1)
                                        {
                                            PlayCommonSound("HitMeleeS");
                                        }
                                        else if (HitEffect.AttackLevel < 3)
                                        {
                                            PlayCommonSound("HitMeleeM");
                                        }
                                        else
                                        {
                                            PlayCommonSound("HitMeleeL");
                                        }								    
								    }
								    else
								    {
								    	if (HitEffect.AttackLevel < 1)
								    	{
								    		CreateCommonParticle("cmn_hit_s", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeS");
								    	}
								    	else if (HitEffect.AttackLevel < 3)
								    	{
								    		CreateCommonParticle("cmn_hit_m", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeM");
								    	}
								    	else
								    	{
								    		CreateCommonParticle("cmn_hit_l", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeL");
								    	}
								    }
								}
								else if (IsPlayer)
								{
								    if (Player->StateMachine->GetCurrentState()->StateType == EStateType::SpecialAttack)
								    {
										CreateCommonParticle("cmn_hit_sp", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
                                        if (HitEffect.AttackLevel < 1)
                                        {
                                            PlayCommonSound("HitMeleeS");
                                        }
                                        else if (HitEffect.AttackLevel < 3)
                                        {
                                            PlayCommonSound("HitMeleeM");
                                        }
                                        else
                                        {
                                            PlayCommonSound("HitMeleeL");
                                        }								    
								    }
								    else
								    {
								    	if (HitEffect.AttackLevel < 1)
								    	{
								    		CreateCommonParticle("cmn_hit_s", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeS");
								    	}
								    	else if (HitEffect.AttackLevel < 3)
								    	{
								    		CreateCommonParticle("cmn_hit_m", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeM");
								    	}
								    	else
								    	{
								    		CreateCommonParticle("cmn_hit_l", POS_Hit, FVector(-50, 0, 0), FRotator(-HitEffect.HitAngle, 0, 0));
								    		PlayCommonSound("HitMeleeL");
								    	}
								    }
								}
							}
							return;
						}
					}
				}
			}
		}
	}
}

void ABattleActor::EnableHit(bool Enabled)
{
	HitActive = Enabled;
}

void ABattleActor::SetAttacking(bool Attacking)
{
	IsAttacking = Attacking;
}

void ABattleActor::SetHitEffect(FHitEffect InHitEffect)
{
	HitEffect = InHitEffect;
}

void ABattleActor::SetCounterHitEffect(FHitEffect InHitEffect)
{
	CounterHitEffect = InHitEffect;
}

void ABattleActor::CreateCommonParticle(FString Name, EPosType PosType, FVector Offset, FRotator Rotation)
{
	AFighterGameState* GameState = Cast<AFighterGameState>(GetWorld()->GetGameState());
	if (Player->CommonParticleData != nullptr)
	{
		for (FParticleStruct ParticleStruct : Player->CommonParticleData->ParticleDatas)
		{
			if (ParticleStruct.Name == Name)
			{
				FVector FinalLocation;
				if (!FacingRight)
					Offset = FVector(Offset.X, -Offset.Y, Offset.Z);
				switch (PosType)
				{
				case POS_Player:
					FinalLocation = Offset + GetActorLocation();
					break;
				case POS_Enemy:
					FinalLocation = Offset + Player->Enemy->GetActorLocation();
					break;
				case POS_Hit:
					FinalLocation = Offset + FVector(0, HitPosX / COORD_SCALE, HitPosY / COORD_SCALE);
					break;
				default:
					FinalLocation = Offset + GetActorLocation();
					break;
				}
				GameState->ParticleManager->NiagaraComponents.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ParticleStruct.ParticleSystem, FinalLocation, Rotation, GetActorScale()));
				GameState->ParticleManager->NiagaraComponents.Last()->SetAgeUpdateMode(ENiagaraAgeUpdateMode::DesiredAge);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableBool("NeedsRollback", true);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableFloat("SpriteRotate", Rotation.Pitch);
				if (!FacingRight)
				{
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableVec2("UVScale", FVector2D(-1, 1));
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableVec2("PivotOffset", FVector2D(0, 0.5));
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableFloat("SpriteRotate", -Rotation.Pitch);
				}
				break;
			}
		}
	}
}

void ABattleActor::CreateCharaParticle(FString Name, EPosType PosType, FVector Offset, FRotator Rotation)
{
	AFighterGameState* GameState = Cast<AFighterGameState>(GetWorld()->GetGameState());
	if (Player->ParticleData != nullptr)
	{
		for (FParticleStruct ParticleStruct : Player->ParticleData->ParticleDatas)
		{
			if (ParticleStruct.Name == Name)
			{
				FVector FinalLocation;
				if (!FacingRight)
					Offset = FVector(Offset.X, -Offset.Y, Offset.Z);
				switch (PosType)
				{
				case POS_Player:
					FinalLocation = Offset + GetActorLocation();
					break;
				case POS_Enemy:
					FinalLocation = Offset + Player->Enemy->GetActorLocation();
					break;
				case POS_Hit:
					FinalLocation = Offset + FVector(0, HitPosX / COORD_SCALE, HitPosY / COORD_SCALE);
					break;
				default:
					FinalLocation = Offset + GetActorLocation();
					break;
				}
				GameState->ParticleManager->NiagaraComponents.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ParticleStruct.ParticleSystem, FinalLocation, Rotation, GetActorScale()));
				GameState->ParticleManager->NiagaraComponents.Last()->SetAgeUpdateMode(ENiagaraAgeUpdateMode::DesiredAge);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableBool("NeedsRollback", true);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableFloat("SpriteRotate", Rotation.Pitch);
				if (!FacingRight)
				{
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableVec2("UVScale", FVector2D(-1, 1));
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableVec2("PivotOffset", FVector2D(0, 1));
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableFloat("SpriteRotate", -Rotation.Pitch);
				}
				break;
			}
		}
	}
}

void ABattleActor::LinkCharaParticle(FString Name)
{
	AFighterGameState* GameState = Cast<AFighterGameState>(GetWorld()->GetGameState());
	if (Player->ParticleData != nullptr)
	{
		for (FParticleStruct ParticleStruct : Player->ParticleData->ParticleDatas)
		{
			if (ParticleStruct.Name == Name)
			{
				FVector Scale;
				if (!FacingRight)
				{
					Scale = FVector(1, -1, 1);
				}
				else
				{
					Scale = FVector(1, 1, 1);
				}
				GameState->ParticleManager->NiagaraComponents.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ParticleStruct.ParticleSystem, GetActorLocation(), FRotator::ZeroRotator, Scale));
				GameState->ParticleManager->NiagaraComponents.Last()->SetAgeUpdateMode(ENiagaraAgeUpdateMode::DesiredAge);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableBool("NeedsRollback", true);
				LinkedParticle = GameState->ParticleManager->NiagaraComponents.Last();
				if (!FacingRight)
					GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableVec2("UVScale", FVector2D(-1, 1));
				break;
			}
		}
	}
}

void ABattleActor::PlayCommonSound(FString Name)
{
	if (Player->CommonSoundData != nullptr)
	{
		for (FSoundDataStruct SoundStruct : Player->CommonSoundData->SoundDatas)
		{
			if (SoundStruct.Name == Name)
			{
				UGameplayStatics::PlaySoundAtLocation(this, SoundStruct.SoundWave, GetActorLocation());
				break;
			}
		}
	}
}

void ABattleActor::PlayCharaSound(FString Name)
{
	if (Player->SoundData != nullptr)
	{
		for (FSoundDataStruct SoundStruct : Player->SoundData->SoundDatas)
		{
			if (SoundStruct.Name == Name)
			{
				UGameplayStatics::PlaySoundAtLocation(this, SoundStruct.SoundWave, GetActorLocation());
				break;
			}
		}
	}
}

void ABattleActor::DeactivateObject()
{
	DeactivateOnNextUpdate = true;
}

void ABattleActor::ResetObject()
{
	ObjectState->OnExit();
	DeactivateOnNextUpdate = false;
	if (LinkedParticle != nullptr)
	{
		LinkedParticle->SetVisibility(false);
	}
	IsActive = false;
	PosX = 0;
	PosY = 0;
	PrevPosX = 0;
	PrevPosY = 0;
	SpeedX = 0;
	SpeedY = 0;
	Gravity = 1900;
	Inertia = 0;
	ActionTime = -1;
	PushHeight = 0;
	PushHeightLow = 0;
	PushWidth = 0;
	Hitstop = 0;
	HitActive = false;
	IsAttacking = false;
	FacingRight = false;
	HasHit = false;
	CelNameInternal.SetString("");
	HitEffectName.SetString("");
	AnimTime = -1;
	AnimBPTime = -1;
}

void ABattleActor::SaveForRollback(unsigned char* Buffer)
{
	FMemory::Memcpy(Buffer, &ObjSync, SIZEOF_BATTLEACTOR);
}

void ABattleActor::LoadForRollback(unsigned char* Buffer)
{
	FMemory::Memcpy(&ObjSync, Buffer, SIZEOF_BATTLEACTOR);
	if (LinkedParticle != nullptr)
	{
		LinkedParticle->SetVisibility(false);
	}
	CelName = FString(CelNameInternal.GetString());
}

void ABattleActor::LogForSyncTest()
{
	UE_LOG(LogTemp, Warning, TEXT("PosX: %d"), PosX);
	UE_LOG(LogTemp, Warning, TEXT("PosY: %d"), PosY);
	UE_LOG(LogTemp, Warning, TEXT("PrevPosX: %d"), PrevPosX);
	UE_LOG(LogTemp, Warning, TEXT("PrevPosY: %d"), PrevPosY);
	UE_LOG(LogTemp, Warning, TEXT("SpeedX: %d"), SpeedX);
	UE_LOG(LogTemp, Warning, TEXT("SpeedY: %d"), SpeedY);
	UE_LOG(LogTemp, Warning, TEXT("Gravity: %d"), Gravity);
	UE_LOG(LogTemp, Warning, TEXT("Inertia: %d"), Inertia);
	UE_LOG(LogTemp, Warning, TEXT("ActionTime: %d"), ActionTime);
	UE_LOG(LogTemp, Warning, TEXT("PushHeight: %d"), PushHeight);
	UE_LOG(LogTemp, Warning, TEXT("PushHeightLow: %d"), PushHeightLow);
	UE_LOG(LogTemp, Warning, TEXT("PushWidth: %d"), PushWidth);
	UE_LOG(LogTemp, Warning, TEXT("Hitstop: %d"), Hitstop);
	UE_LOG(LogTemp, Warning, TEXT("CelName: %s"), *FString(CelNameInternal.GetString()))
	UE_LOG(LogTemp, Warning, TEXT("HitActive: %d"), HitActive);
	UE_LOG(LogTemp, Warning, TEXT("IsAttacking: %d"), IsAttacking);
	UE_LOG(LogTemp, Warning, TEXT("FacingRight: %d"), FacingRight);
	UE_LOG(LogTemp, Warning, TEXT("HasHit: %d"), HasHit);
	UE_LOG(LogTemp, Warning, TEXT("MiscFlags: %d"), MiscFlags);
	UE_LOG(LogTemp, Warning, TEXT("AnimTime: %d"), AnimTime);
	UE_LOG(LogTemp, Warning, TEXT("AnimBPTime: %d"), AnimBPTime);
	UE_LOG(LogTemp, Warning, TEXT("DefaultCommonAction: %d"), DefaultCommonAction);
}

void ABattleActor::LogForSyncTestFile(FILE* file)
{
	if(file)
	{
		fprintf(file,"BattleActor:\n");
		fprintf(file,"\tPosX: %d\n", PosX);
		fprintf(file,"\tPosY: %d\n", PosY);
		fprintf(file,"\tPrevPosX: %d\n", PrevPosX);
		fprintf(file,"\tPrevPosY: %d\n", PrevPosY);
		fprintf(file,"\tSpeedX: %d\n", SpeedX);
		fprintf(file,"\tSpeedY: %d\n", SpeedY);
		fprintf(file,"\tGravity: %d\n", Gravity);
		fprintf(file,"\tInertia: %d\n", Inertia);
		fprintf(file,"\tActionTime: %d\n", ActionTime);
		fprintf(file,"\tPushHeight: %d\n", PushHeight);
		fprintf(file,"\tPushHeightLow: %d\n", PushHeightLow);
		fprintf(file,"\tPushWidth: %d\n", PushWidth);
		fprintf(file,"\tHitstop: %d\n", Hitstop);
		fprintf(file,"\tCelName: %s\n", CelNameInternal.GetString());
		fprintf(file,"\tHitActive: %d\n", HitActive);
		fprintf(file,"\tIsAttacking: %d\n", IsAttacking);
		fprintf(file,"\tFacingRight: %d\n", FacingRight);
		fprintf(file,"\tHasHit: %d\n", HasHit);
		fprintf(file,"\tMiscFlags: %d\n", MiscFlags);
		fprintf(file,"\tAnimTime: %d\n", AnimTime);
		fprintf(file,"\tAnimBPTime: %d\n", AnimBPTime);
		fprintf(file,"\tDefaultCommonAction: %d\n", DefaultCommonAction);
		fprintf(file,"\tObjectState: %p\n", ObjectState);
	}
}
