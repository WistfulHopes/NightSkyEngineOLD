// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealBattleActor.h"
#include "Battle/Actors/BattleActor.h"
#include "UnrealFighterGameState.h"
#include "NiagaraComponent.h"
#include "UnrealPlayerCharacter.h"
#include "Battle/Actors/PlayerCharacter.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"

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
}

void ABattleActor::CreateCommonParticleCallback(char* Name, PosType PosType, Vector Offset, int32_t Angle)
{
	CreateCommonParticle(Name, (EPosType)PosType, FVector(0, Offset.X, Offset.Y), FRotator(0, 0, Angle));
}

void ABattleActor::CreateCharaParticleCallback(char* Name, PosType PosType, Vector Offset, int32_t Angle)
{
	CreateCharaParticle(Name, (EPosType)PosType, FVector(0, Offset.X, Offset.Y), FRotator(0, 0, Angle));
}

void ABattleActor::LinkCharaParticleCallback(char* Name)
{
	LinkCharaParticle(Name);
}

void ABattleActor::PlayCommonSoundCallback(char* Name)
{
	PlayCommonSound(Name);
}

void ABattleActor::PlayCharaSoundCallback(char* Name)
{
	PlayCharaSound(Name);
}

void ABattleActor::CreateCallbacks()
{
	Parent.Get()->CreateCommonParticle = std::bind(&ABattleActor::CreateCommonParticleCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	Parent.Get()->CreateCharaParticle = std::bind(&ABattleActor::CreateCharaParticleCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	Parent.Get()->LinkCharaParticle = std::bind(&ABattleActor::LinkCharaParticleCallback, this, std::placeholders::_1);
	Parent.Get()->PlayCommonSound = std::bind(&ABattleActor::PlayCommonSoundCallback, this, std::placeholders::_1);
	Parent.Get()->PlayCharaSound = std::bind(&ABattleActor::PlayCharaSoundCallback, this, std::placeholders::_1);
}

// Called every frame
void ABattleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Parent.IsValid())
	{
		if (Parent.Get()->IsActive)
		{
			if (!Parent.Get()->FacingRight)
			{
				SetActorScale3D(FVector(1, -1, 1));
			}
			else
			{
				SetActorScale3D(FVector(1, 1, 1));
			}
			SetActorLocation(FVector(0, float(Parent.Get()->GetInternalValue(VAL_PosX)) / COORD_SCALE, float(Parent.Get()->GetInternalValue(VAL_PosY)) / COORD_SCALE)); //set visual location and scale in unreal
				
			if (LinkedParticle != nullptr)
			{
				LinkedParticle->SetWorldLocation(FVector(0, float(Parent.Get()->GetInternalValue(VAL_PosX)) / COORD_SCALE, float(Parent.Get()->GetInternalValue(VAL_PosY)) / COORD_SCALE));
				LinkedParticle->SetVisibility(true);
			}
		}
		else
		{
			TArray<UActorComponent*> Components;
			GetComponents(Components);
			for (auto Component : Components)
			{
				Component->Deactivate();
				Component->DestroyComponent();
			}
			if (IsValid(LinkedParticle))
				LinkedParticle->DestroyComponent();
		}
	}
}

BattleActor* ABattleActor::GetParent()
{
	return Parent.Get();
}

void ABattleActor::SetParent(BattleActor* InActor)
{
	Parent = TSharedPtr<BattleActor>(InActor);
}

void ABattleActor::Update()
{
	Parent.Get()->StateVal1 = StateVal1;
	Parent.Get()->StateVal2 = StateVal2;
	Parent.Get()->StateVal3 = StateVal3;
	Parent.Get()->StateVal4 = StateVal4;
	Parent.Get()->StateVal5 = StateVal5;
	Parent.Get()->StateVal6 = StateVal6;
	Parent.Get()->StateVal7 = StateVal7;
	Parent.Get()->StateVal8 = StateVal8;
	Parent.Get()->AnimTime = AnimTime;
	Parent.Get()->SkelAnimTime = AnimBPTime;
	Parent.Get()->DefaultCommonAction = DefaultCommonAction;
}

void ABattleActor::PreUpdate()
{
	StateVal1 = Parent.Get()->StateVal1;
	StateVal2 = Parent.Get()->StateVal2;
	StateVal3 = Parent.Get()->StateVal3;
    StateVal4 = Parent.Get()->StateVal4;
    StateVal5 = Parent.Get()->StateVal5;
	StateVal6 = Parent.Get()->StateVal6;
    StateVal7 = Parent.Get()->StateVal7;
    StateVal8 = Parent.Get()->StateVal8;
    AnimTime = Parent.Get()->AnimTime;
    AnimBPTime = Parent.Get()->SkelAnimTime;
    DefaultCommonAction = Parent.Get()->DefaultCommonAction;
}

void ABattleActor::FixObjectStateForRollback()
{
	for (auto InPlayer : GameState->Players)
	{
		if (InPlayer->GetParent() == Parent.Get()->Player)
		{
			Player = InPlayer;
		}
	}
	int StateIndex = Player->ObjectStateNames.Find(Parent.Get()->ObjectStateName.GetString());
	if (StateIndex != INDEX_NONE)
	{
		ObjectState = DuplicateObject(Player->ObjectStates[StateIndex], this);
		ObjectState->ObjectParent = this;
		reinterpret_cast<BlueprintState*>(GetParent()->ObjectState)->Owner = ObjectState;
	}
}

void ABattleActor::SetPosX(int InPosX)
{
	Parent.Get()->SetPosX(InPosX);
}

void ABattleActor::SetPosY(int InPosY)
{
	Parent.Get()->SetPosY(InPosY);
}

void ABattleActor::AddPosX(int InPosX)
{
	Parent.Get()->AddPosX(InPosX);
}

void ABattleActor::AddPosXRaw(int InPosX)
{
	Parent.Get()->AddPosXRaw(InPosX);
}

void ABattleActor::AddPosY(int InPosY)
{
	Parent.Get()->AddPosY(InPosY);
}

void ABattleActor::SetSpeedX(int InSpeedX)
{
	Parent.Get()->SetSpeedX(InSpeedX);
}

void ABattleActor::SetSpeedY(int InSpeedY)
{
	Parent.Get()->SetSpeedY(InSpeedY);
}

void ABattleActor::SetGravity(int InGravity)
{
	Parent.Get()->SetGravity(InGravity);
}

void ABattleActor::HaltMomentum()
{
	Parent.Get()->HaltMomentum();
}

void ABattleActor::SetPushWidthExpand(int Expand)
{
	Parent.Get()->SetPushWidthExpand(Expand);
}

int ABattleActor::GetInternalValue(EInternalValue InInternalValue, EObjType InObjType)
{
	return Parent.Get()->GetInternalValue((InternalValue)InInternalValue, (ObjType)InObjType);
}

bool ABattleActor::IsOnFrame(int Frame)
{
	if (Frame == AnimTime)
	{
		return true;
	}
	return false;
}

bool ABattleActor::IsStopped()
{
	return Parent.Get()->IsStopped();
}

void ABattleActor::SetCelName(FString InCelName)
{
	CelName = InCelName;
	Parent.Get()->SetCelName(TCHAR_TO_ANSI(*InCelName));
}

void ABattleActor::SetHitEffectName(FString InHitEffectName)
{
	Parent.Get()->SetHitEffectName(TCHAR_TO_ANSI(*InHitEffectName));
}

void ABattleActor::AddSpeedX(int InSpeedX)
{
	Parent.Get()->AddSpeedX(InSpeedX);
}

void ABattleActor::AddSpeedY(int InSpeedY)
{
	Parent.Get()->AddSpeedY(InSpeedY);
}

void ABattleActor::SetSpeedXPercent(int32 Percent)
{
	Parent.Get()->SetSpeedXPercent(Percent);
}

void ABattleActor::SetSpeedXPercentPerFrame(int32 Percent)
{
	Parent.Get()->SetSpeedXPercentPerFrame(Percent);
}

void ABattleActor::SetInertia(int InInertia)
{
	Parent.Get()->SetInertia(InInertia);
}

void ABattleActor::ClearInertia()
{
	Parent.Get()->ClearInertia();
}

void ABattleActor::EnableInertia()
{
	Parent.Get()->EnableInertia();
}

void ABattleActor::DisableInertia()
{
	Parent.Get()->DisableInertia();
}

void ABattleActor::SetFacing(bool NewFacingRight)
{
	Parent.Get()->SetFacing(NewFacingRight);
}

void ABattleActor::FlipCharacter()
{
	Parent.Get()->FlipCharacter();
}

void ABattleActor::EnableFlip(bool Enabled)
{
	Parent.Get()->EnableFlip(Enabled);
}

void ABattleActor::GetBoxes()
{
	CollisionBoxes.Empty();
	if (Player->CollisionData != nullptr)
	{
		for (int i = 0; i < Player->CollisionData->CollisionFrames.Num(); i++)
		{
			if (Player->CollisionData->CollisionFrames[i].Name == Parent.Get()->CelNameInternal.GetString())
			{
				for (int j = 0; j < CollisionArraySize; j++)
				{
					if (j < Player->CollisionData->CollisionFrames[i].CollisionBoxes.Num())
					{
						CollisionBox CollisionBoxInternal;
						CollisionBoxInternal.Type = (BoxType)Player->CollisionData->CollisionFrames[i].CollisionBoxes[j]->Type;
						CollisionBoxInternal.PosX = Player->CollisionData->CollisionFrames[i].CollisionBoxes[j]->PosX;
						CollisionBoxInternal.PosY = Player->CollisionData->CollisionFrames[i].CollisionBoxes[j]->PosY;
						CollisionBoxInternal.SizeX = Player->CollisionData->CollisionFrames[i].CollisionBoxes[j]->SizeX;
						CollisionBoxInternal.SizeY = Player->CollisionData->CollisionFrames[i].CollisionBoxes[j]->SizeY;
						CollisionBoxes.Add(CollisionBoxInternal);
						Parent.Get()->CollisionBoxes[j] = CollisionBoxInternal;
					}
					else
					{
						Parent.Get()->CollisionBoxes[j].Type = Hurtbox;
						Parent.Get()->CollisionBoxes[j].PosX = -10000000;
						Parent.Get()->CollisionBoxes[j].PosY = -10000000;
						Parent.Get()->CollisionBoxes[j].SizeX = 0;
						Parent.Get()->CollisionBoxes[j].SizeY = 0;
					}
				}
				return;
			}
		}
		for (int j = 0; j < CollisionArraySize; j++)
		{
			Parent.Get()->CollisionBoxes[j].Type = Hurtbox;
			Parent.Get()->CollisionBoxes[j].PosX = -10000000;
			Parent.Get()->CollisionBoxes[j].PosY = -10000000;
			Parent.Get()->CollisionBoxes[j].SizeX = 0;
			Parent.Get()->CollisionBoxes[j].SizeY = 0;
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
	int PosX = Parent.Get()->GetInternalValue(InternalValue::VAL_PosX);
	int PosY = Parent.Get()->GetInternalValue(InternalValue::VAL_PosY);
	if (CollisionBoxes.Num() != 0)
	{
		for (int i = 0; i < CollisionBoxes.Num(); i++)
		{
			TArray<FVector2D> CurrentCorners;
			if (Parent.Get()->FacingRight)
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
			if (CollisionBoxes[i].Type == BoxType::Hitbox)
				color = FLinearColor(1.f, 0.f, 0.f, .25f);
			else if (Parent.Get()->IsAttacking)
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
	CurrentCorners.Add(FVector2D(Parent.Get()->L / COORD_SCALE, Parent.Get()->B / COORD_SCALE));
	CurrentCorners.Add(FVector2D(Parent.Get()->R / COORD_SCALE, Parent.Get()->B / COORD_SCALE));
	CurrentCorners.Add(FVector2D(Parent.Get()->R / COORD_SCALE, Parent.Get()->T / COORD_SCALE));
	CurrentCorners.Add(FVector2D(Parent.Get()->L / COORD_SCALE, Parent.Get()->T / COORD_SCALE));
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

void ABattleActor::EnableHit(bool Enabled)
{
	Parent.Get()->EnableHit(Enabled);
}

void ABattleActor::SetAttacking(bool Attacking)
{
	Parent.Get()->SetAttacking(Attacking);
}

void ABattleActor::SetHeadAttribute(bool HeadAttribute)
{
	Parent.Get()->SetHeadAttribute(HeadAttribute);
}

void ABattleActor::SetHitEffect(FHitEffect InHitEffect)
{
	HitEffect InternalHitEFfect;
	InternalHitEFfect.AttackLevel = InHitEffect.AttackLevel;
	InternalHitEFfect.BlockType = (BlockType)InHitEffect.BlockType;
	InternalHitEFfect.Hitstun = InHitEffect.Hitstun;
	InternalHitEFfect.Blockstun = InHitEffect.Blockstun;
	InternalHitEFfect.Untech = InHitEffect.Untech;
	InternalHitEFfect.Hitstop = InHitEffect.Hitstop;
	InternalHitEFfect.HitDamage = InHitEffect.HitDamage;
	InternalHitEFfect.MinimumDamagePercent = InHitEffect.MinimumDamagePercent;
	InternalHitEFfect.ChipDamagePercent = InHitEffect.ChipDamagePercent;
	InternalHitEFfect.InitialProration = InHitEffect.InitialProration;
	InternalHitEFfect.ForcedProration = InHitEffect.ForcedProration;
	InternalHitEFfect.HitPushbackX = InHitEffect.HitPushbackX;
	InternalHitEFfect.AirHitPushbackX = InHitEffect.AirHitPushbackX;
	InternalHitEFfect.AirHitPushbackY = InHitEffect.AirHitPushbackY;
	InternalHitEFfect.HitGravity = InHitEffect.HitGravity;
	InternalHitEFfect.HitAngle = InHitEffect.HitAngle;
	InternalHitEFfect.GroundHitAction = (HitAction)InHitEffect.GroundHitAction;
	InternalHitEFfect.AirHitAction = (HitAction)InHitEffect.AirHitAction;
	InternalHitEFfect.KnockdownTime = InHitEffect.KnockdownTime;

	GroundBounceEffect InternalGroundBounceEffect;
	InternalGroundBounceEffect.GroundBounceCount = InHitEffect.GroundBounceEffect.GroundBounceCount;
	InternalGroundBounceEffect.GroundBounceUntech = InHitEffect.GroundBounceEffect.GroundBounceUntech;
	InternalGroundBounceEffect.GroundBounceXSpeed = InHitEffect.GroundBounceEffect.GroundBounceXSpeed;
	InternalGroundBounceEffect.GroundBounceYSpeed = InHitEffect.GroundBounceEffect.GroundBounceYSpeed;
	InternalGroundBounceEffect.GroundBounceGravity = InHitEffect.GroundBounceEffect.GroundBounceGravity;
	InternalHitEFfect.GroundBounceEffect = InternalGroundBounceEffect;

	WallBounceEffect InternalWallBounceEffect;
	InternalWallBounceEffect.WallBounceCount = InHitEffect.WallBounceEffect.WallBounceCount;
	InternalWallBounceEffect.WallBounceUntech = InHitEffect.WallBounceEffect.WallBounceUntech;
	InternalWallBounceEffect.WallBounceXSpeed = InHitEffect.WallBounceEffect.WallBounceXSpeed;
	InternalWallBounceEffect.WallBounceYSpeed = InHitEffect.WallBounceEffect.WallBounceYSpeed;
	InternalWallBounceEffect.WallBounceGravity = InHitEffect.WallBounceEffect.WallBounceGravity;
	InternalWallBounceEffect.WallBounceInCornerOnly = InHitEffect.WallBounceEffect.WallBounceInCornerOnly;
	InternalHitEFfect.WallBounceEffect = InternalWallBounceEffect;

	InternalHitEFfect.SFXType = (HitSFXType)InHitEffect.SFXType;
	InternalHitEFfect.VFXType = (HitVFXType)InHitEffect.VFXType;
	InternalHitEFfect.DeathCamOverride = InHitEffect.DeathCamOverride;

	Parent.Get()->SetHitEffect(InternalHitEFfect);
}

void ABattleActor::SetCounterHitEffect(FHitEffect InHitEffect)
{

	HitEffect InternalHitEFfect;
	InternalHitEFfect.AttackLevel = InHitEffect.AttackLevel;
	InternalHitEFfect.BlockType = (BlockType)InHitEffect.BlockType;
	InternalHitEFfect.Hitstun = InHitEffect.Hitstun;
	InternalHitEFfect.Blockstun = InHitEffect.Blockstun;
	InternalHitEFfect.Untech = InHitEffect.Untech;
	InternalHitEFfect.Hitstop = InHitEffect.Hitstop;
	InternalHitEFfect.HitDamage = InHitEffect.HitDamage;
	InternalHitEFfect.MinimumDamagePercent = InHitEffect.MinimumDamagePercent;
	InternalHitEFfect.ChipDamagePercent = InHitEffect.ChipDamagePercent;
	InternalHitEFfect.InitialProration = InHitEffect.InitialProration;
	InternalHitEFfect.ForcedProration = InHitEffect.ForcedProration;
	InternalHitEFfect.HitPushbackX = InHitEffect.HitPushbackX;
	InternalHitEFfect.AirHitPushbackX = InHitEffect.AirHitPushbackX;
	InternalHitEFfect.AirHitPushbackY = InHitEffect.AirHitPushbackY;
	InternalHitEFfect.HitGravity = InHitEffect.HitGravity;
	InternalHitEFfect.HitAngle = InHitEffect.HitAngle;
	InternalHitEFfect.GroundHitAction = (HitAction)InHitEffect.GroundHitAction;
	InternalHitEFfect.AirHitAction = (HitAction)InHitEffect.AirHitAction;
	InternalHitEFfect.KnockdownTime = InHitEffect.KnockdownTime;

	GroundBounceEffect InternalGroundBounceEffect;
	InternalGroundBounceEffect.GroundBounceCount = InHitEffect.GroundBounceEffect.GroundBounceCount;
	InternalGroundBounceEffect.GroundBounceUntech = InHitEffect.GroundBounceEffect.GroundBounceUntech;
	InternalGroundBounceEffect.GroundBounceXSpeed = InHitEffect.GroundBounceEffect.GroundBounceXSpeed;
	InternalGroundBounceEffect.GroundBounceYSpeed = InHitEffect.GroundBounceEffect.GroundBounceYSpeed;
	InternalGroundBounceEffect.GroundBounceGravity = InHitEffect.GroundBounceEffect.GroundBounceGravity;
	InternalHitEFfect.GroundBounceEffect = InternalGroundBounceEffect;

	WallBounceEffect InternalWallBounceEffect;
	InternalWallBounceEffect.WallBounceCount = InHitEffect.WallBounceEffect.WallBounceCount;
	InternalWallBounceEffect.WallBounceUntech = InHitEffect.WallBounceEffect.WallBounceUntech;
	InternalWallBounceEffect.WallBounceXSpeed = InHitEffect.WallBounceEffect.WallBounceXSpeed;
	InternalWallBounceEffect.WallBounceYSpeed = InHitEffect.WallBounceEffect.WallBounceYSpeed;
	InternalWallBounceEffect.WallBounceGravity = InHitEffect.WallBounceEffect.WallBounceGravity;
	InternalWallBounceEffect.WallBounceInCornerOnly = InHitEffect.WallBounceEffect.WallBounceInCornerOnly;
	InternalHitEFfect.WallBounceEffect = InternalWallBounceEffect;

	InternalHitEFfect.SFXType = (HitSFXType)InHitEffect.SFXType;
	InternalHitEFfect.VFXType = (HitVFXType)InHitEffect.VFXType;
	InternalHitEFfect.DeathCamOverride = InHitEffect.DeathCamOverride;

	Parent.Get()->SetCounterHitEffect(InternalHitEFfect);}

void ABattleActor::CreateCommonParticle(FString Name, EPosType PosType, FVector Offset, FRotator Rotation)
{
	if (Player->CommonParticleData != nullptr)
	{
		for (FParticleStruct ParticleStruct : Player->CommonParticleData->ParticleDatas)
		{
			if (ParticleStruct.Name == Name)
			{
				FVector FinalLocation;
				if (!Parent.Get()->FacingRight)
					Offset = FVector(Offset.X, -Offset.Y, Offset.Z);
				switch (PosType)
				{
				case EPosType::POS_Player:
					FinalLocation = Offset + GetActorLocation();
					break;
				case EPosType::POS_Enemy:
					FinalLocation = Offset + Player->Enemy->GetActorLocation();
					break;
				case EPosType::POS_Hit:
					FinalLocation = Offset + FVector(0, Parent.Get()->HitPosX / COORD_SCALE, Parent.Get()->HitPosY / COORD_SCALE);
					break;
				default:
					FinalLocation = Offset + GetActorLocation();
					break;
				}
				GameState->ParticleManager->NiagaraComponents.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ParticleStruct.ParticleSystem, FinalLocation, Rotation, GetActorScale()));
				GameState->ParticleManager->NiagaraComponents.Last()->SetAgeUpdateMode(ENiagaraAgeUpdateMode::DesiredAge);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableBool("NeedsRollback", true);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableFloat("SpriteRotate", Rotation.Pitch);
				if (!Parent.Get()->FacingRight)
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
	if (Player->ParticleData != nullptr)
	{
		for (FParticleStruct ParticleStruct : Player->ParticleData->ParticleDatas)
		{
			if (ParticleStruct.Name == Name)
			{
				FVector FinalLocation;
				if (!Parent.Get()->FacingRight)
					Offset = FVector(Offset.X, -Offset.Y, Offset.Z);
				switch (PosType)
				{
				case EPosType::POS_Player:
					FinalLocation = Offset + GetActorLocation();
					break;
				case EPosType::POS_Enemy:
					FinalLocation = Offset + Player->Enemy->GetActorLocation();
					break;
				case EPosType::POS_Hit:
					FinalLocation = Offset + FVector(0, Parent.Get()->HitPosX / COORD_SCALE, Parent.Get()->HitPosY / COORD_SCALE);
					break;
				default:
					FinalLocation = Offset + GetActorLocation();
					break;
				}
				GameState->ParticleManager->NiagaraComponents.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ParticleStruct.ParticleSystem, FinalLocation, Rotation, GetActorScale()));
				GameState->ParticleManager->NiagaraComponents.Last()->SetAgeUpdateMode(ENiagaraAgeUpdateMode::DesiredAge);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableBool("NeedsRollback", true);
				GameState->ParticleManager->NiagaraComponents.Last()->SetNiagaraVariableFloat("SpriteRotate", Rotation.Pitch);
				if (!Parent.Get()->FacingRight)
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
	if (Player->ParticleData != nullptr)
	{
		for (FParticleStruct ParticleStruct : Player->ParticleData->ParticleDatas)
		{
			if (ParticleStruct.Name == Name)
			{
				FVector Scale;
				if (!Parent.Get()->FacingRight)
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
				if (!!Parent.Get()->FacingRight)
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
				GameState->PlayCommonAudio(SoundStruct.SoundWave, SoundStruct.MaxDuration);
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
				GameState->PlayCharaAudio(SoundStruct.SoundWave, SoundStruct.MaxDuration);
				break;
			}
		}
	}
}

void ABattleActor::AttachToSocketOfObject(FString InSocketName, FVector Offset, EObjType ObjType)
{
	SocketName = InSocketName;
	SocketObj = ObjType;
	SocketOffset = Offset;
}

void ABattleActor::DetachFromSocket()
{
	SocketName = "";
	SocketObj = EObjType::OBJ_Self;
}

void ABattleActor::PauseRoundTimer(bool Pause)
{
	Parent.Get()->PauseRoundTimer(Pause);
}

void ABattleActor::SetObjectID(int InObjectID)
{
	Parent.Get()->SetObjectID(InObjectID);
}

void ABattleActor::DeactivateIfBeyondBounds()
{
	Parent.Get()->DeactivateIfBeyondBounds();
}

void ABattleActor::DeactivateObject()
{
	Parent.Get()->DeactivateObject();
}

ABattleActor* ABattleActor::GetBattleActor(EObjType Type)
{
	BattleActor* TmpActor = Parent.Get()->GetBattleActor((ObjType)Type);
	if (!TmpActor)
		return nullptr;
	
	for (int i = 0; i < 400; i++)
	{
		if (GameState->Objects[i]->GetParent() == TmpActor)
		{
			return GameState->Objects[i];
		}
	}
	for (int i = 0; i < 6; i++)
	{
		if (GameState->Players[i]->GetParent() == TmpActor)
		{
			return GameState->Objects[i];
		}
	}
	return nullptr;
}


