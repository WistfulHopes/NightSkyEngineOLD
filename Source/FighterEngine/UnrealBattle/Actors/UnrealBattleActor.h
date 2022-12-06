// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/CollisionBox.h"
#include "Battle/Actors/BattleActor.h"
#include "FighterEngine/UnrealBattle/CollisionBoxUnreal.h"
#include "FighterEngine/DataAssets/CollisionData.h"
#include "UnrealBattleActor.generated.h"

class UNiagaraComponent;
class UState;
class APlayerCharacter;
class AFighterGameState;

#define COORD_SCALE ((double)1000 / 0.43)

UENUM()
enum class EPosType : uint8
{
	POS_Player,
	POS_Enemy,
	POS_Hit,
};

UENUM()
enum class EObjType : uint8
{
	OBJ_Self,
	OBJ_Enemy,
	OBJ_Parent,
	OBJ_Child0,
	OBJ_Child1,
	OBJ_Child2,
	OBJ_Child3,
	OBJ_Child4,
	OBJ_Child5,
	OBJ_Child6,
	OBJ_Child7,
	OBJ_Child8,
	OBJ_Child9,
	OBJ_Child10,
	OBJ_Child11,
	OBJ_Child12,
	OBJ_Child13,
	OBJ_Child14,
	OBJ_Child15,
};

UENUM()
enum class EHitSFXType : uint8
{
	SFX_Punch,
	SFX_Kick,
	SFX_Slash,
};

UENUM()
enum class EHitVFXType : uint8
{
	VFX_Strike,
	VFX_Slash,
};

UENUM() 
enum class EInternalValue : uint8 //internal values list
{
	VAL_StoredRegister,
	VAL_Angle,
	VAL_ActionFlag,
	VAL_PlayerVal1,
	VAL_PlayerVal2,
	VAL_PlayerVal3,
	VAL_PlayerVal4,
	VAL_PlayerVal5,
	VAL_PlayerVal6,
	VAL_PlayerVal7,
	VAL_PlayerVal8,
	VAL_SpeedX,
	VAL_SpeedY,
	VAL_ActionTime,
	VAL_PosX,
	VAL_PosY,
	VAL_Inertia,
	VAL_FacingRight,
	VAL_DistanceToFrontWall,
	VAL_DistanceToBackWall,
	VAL_IsAir,
	VAL_IsLand,
	VAL_IsStunned,
	VAL_Health,
	VAL_Meter,
	VAL_DefaultCommonAction,
};

UENUM()
enum class EHitAction : uint8
{
	HACT_None,
	HACT_GroundNormal,
	HACT_AirNormal,
	HACT_Crumple,
	HACT_ForceCrouch,
	HACT_ForceStand,
	HACT_GuardBreakStand,
	HACT_GuardBreakCrouch,
	HACT_AirFaceUp,
	HACT_AirVertical,
	HACT_AirFaceDown,
	HACT_Blowback,
};

UENUM()
enum class EBlockType : uint8
{
	BLK_Mid,
	BLK_High,
	BLK_Low,
	BLK_None,
};

USTRUCT(BlueprintType)
struct FWallBounceEffect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int WallBounceCount = 0;
	UPROPERTY(BlueprintReadWrite)
	int WallBounceUntech = 0;
	UPROPERTY(BlueprintReadWrite)
	int WallBounceXSpeed;
	UPROPERTY(BlueprintReadWrite)
	int WallBounceYSpeed;
	UPROPERTY(BlueprintReadWrite)
	int WallBounceGravity = 1900;
	UPROPERTY(BlueprintReadWrite)
	bool WallBounceInCornerOnly = false;
};

USTRUCT(BlueprintType)
struct FGroundBounceEffect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int GroundBounceCount = 0;
	UPROPERTY(BlueprintReadWrite)
	int GroundBounceUntech = 0;
	UPROPERTY(BlueprintReadWrite)
	int GroundBounceXSpeed;
	UPROPERTY(BlueprintReadWrite)
	int GroundBounceYSpeed;
	UPROPERTY(BlueprintReadWrite)
	int GroundBounceGravity = 1900;
};

USTRUCT(BlueprintType)
struct FHitEffect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int AttackLevel; //in this engine, it's only used to define effects on hit or block
	UPROPERTY(BlueprintReadWrite)
	EBlockType BlockType;
	UPROPERTY(BlueprintReadWrite)
	int Hitstun;
	UPROPERTY(BlueprintReadWrite)
	int Blockstun;
	UPROPERTY(BlueprintReadWrite)
	int Untech;
	UPROPERTY(BlueprintReadWrite)
	int Hitstop;
	UPROPERTY(BlueprintReadWrite)
	int HitDamage;
	UPROPERTY(BlueprintReadWrite)
	int MinimumDamagePercent;
	UPROPERTY(BlueprintReadWrite)
	int ChipDamagePercent;
	UPROPERTY(BlueprintReadWrite)
	int InitialProration = 100;
	UPROPERTY(BlueprintReadWrite)
	int ForcedProration = 100;
	UPROPERTY(BlueprintReadWrite)
	int HitPushbackX;
	UPROPERTY(BlueprintReadWrite)
	int AirHitPushbackX;
	UPROPERTY(BlueprintReadWrite)
	int AirHitPushbackY;
	UPROPERTY(BlueprintReadWrite)
	int HitGravity = 1900;
	UPROPERTY(BlueprintReadWrite)
	int HitAngle;
	UPROPERTY(BlueprintReadWrite)
	EHitAction GroundHitAction;
	UPROPERTY(BlueprintReadWrite)
	EHitAction AirHitAction;
	UPROPERTY(BlueprintReadWrite)
	int KnockdownTime = 25;
	UPROPERTY(BlueprintReadWrite)
	FGroundBounceEffect GroundBounceEffect;
	UPROPERTY(BlueprintReadWrite)
	FWallBounceEffect WallBounceEffect;
	UPROPERTY(BlueprintReadWrite)
	EHitSFXType SFXType = EHitSFXType::SFX_Punch;
	UPROPERTY(BlueprintReadWrite)
	EHitVFXType VFXType = EHitVFXType::VFX_Strike;
	UPROPERTY(BlueprintReadWrite)
	bool DeathCamOverride;
};

UCLASS()
class FIGHTERENGINE_API ABattleActor : public APawn
{
	GENERATED_BODY()
	
public:
	ABattleActor();
	
	TSharedPtr<BattleActor> Parent;

	UPROPERTY(BlueprintReadWrite)
	int StateVal1;
	UPROPERTY(BlueprintReadWrite)
	int StateVal2;
	UPROPERTY(BlueprintReadWrite)
	int StateVal3;
	UPROPERTY(BlueprintReadWrite)
	int StateVal4;
	UPROPERTY(BlueprintReadWrite)
	int StateVal5;
	UPROPERTY(BlueprintReadWrite)
	int StateVal6;
	UPROPERTY(BlueprintReadWrite)
	int StateVal7;
	UPROPERTY(BlueprintReadWrite)
	int StateVal8;

	FString SocketName; 
	EObjType SocketObj = EObjType::OBJ_Self;
	FVector SocketOffset;
	
	//current animation time
	UPROPERTY(BlueprintReadWrite)
	int AnimTime = -1;

	//animbp time
	UPROPERTY(BlueprintReadWrite)
	int AnimBPTime = -1;

	//for common actions: does it inherit default values?
	UPROPERTY(BlueprintReadWrite)
	bool DefaultCommonAction = true;
	
	//pointer to player. if this is not a player, it will point to the owning player.
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* Player; 
	
	UPROPERTY()
	AFighterGameState* GameState;

	//for use with non-character objects only. sets the state for it to use
	UPROPERTY()
	UState* ObjectState; 

	//only used for rendering collision
	UPROPERTY(EditAnywhere)
	UMaterial* BoxMaterial; 
	
	//cel name. used for animation and collision data calls.
	UPROPERTY(BlueprintReadOnly)
	FString CelName; 

	//grabbed collision boxes from collision data
	TArray<CollisionBox> CollisionBoxes; 
	
	//collision data asset
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCollisionData* CollisionData; 

	//non-player objects only. particle that moves with the object.
	UPROPERTY(BlueprintReadWrite)
	UNiagaraComponent* LinkedParticle;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void CreateCommonParticleCallback(char* Name, PosType PosType, Vector Offset = Vector(0, 0), int32_t Angle = 0);
	void CreateCharaParticleCallback(char* Name, PosType PosType, Vector Offset = Vector(0, 0), int32_t Angle = 0);
	void LinkCharaParticleCallback(char* Name);
	void PlayCommonSoundCallback(char* Name);
	void PlayCharaSoundCallback(char* Name);
	
public:
	virtual void Tick(float DeltaTime) override;
	//get boxes based on cel name
	void GetBoxes(); 

	virtual void CreateCallbacks();

	BattleActor* GetParent();
	void SetParent(BattleActor* InActor);

	//internal functions
	virtual	void Update();
	virtual void PreUpdate();
	
	void FixObjectStateForRollback();
	
	//bp callable functions
	
	//gets internal value for bp
	UFUNCTION(BlueprintPure)
	int GetInternalValue(EInternalValue InInternalValue, EObjType InObjType = EObjType::OBJ_Self);
	//checks if on frame
	UFUNCTION(BlueprintPure)
	bool IsOnFrame(int Frame);
	UFUNCTION(BlueprintPure)
	bool IsStopped();
	//sets cel name
	UFUNCTION(BlueprintCallable)
	void SetCelName(FString InCelName);
	//sets custom hit effect name
	UFUNCTION(BlueprintCallable)
	void SetHitEffectName(FString InHitEffectName);
	//sets x position
	UFUNCTION(BlueprintCallable)
	void SetPosX(int InPosX);
	//sets y position
	UFUNCTION(BlueprintCallable)
	void SetPosY(int InPosY);
	//adds x position depending on direction
	UFUNCTION(BlueprintCallable)
	void AddPosX(int InPosX);
	//adds x position with no regard for direction
	UFUNCTION(BlueprintCallable)
	void AddPosXRaw(int InPosX);
	//adds y position
	UFUNCTION(BlueprintCallable)
	void AddPosY(int InPosY);
	//sets x speed
	UFUNCTION(BlueprintCallable)
	void SetSpeedX(int InSpeedX);
	//sets y speed
	UFUNCTION(BlueprintCallable)
	void SetSpeedY(int InSpeedY);
	//adds x speed
	UFUNCTION(BlueprintCallable)
	void AddSpeedX(int InSpeedX);
	//adds y speed
	UFUNCTION(BlueprintCallable)
	void AddSpeedY(int InSpeedY);
	//the current x speed will be set to this percent.
	UFUNCTION(BlueprintCallable)
	void SetSpeedXPercent(int32 Percent);
	//the current x speed will be set to this percent every frame.
	UFUNCTION(BlueprintCallable)
	void SetSpeedXPercentPerFrame(int32 Percent);
	//sets gravity
	UFUNCTION(BlueprintCallable)
	void SetGravity(int InGravity);
	//sets inertia. when inertia is enabled, inertia adds to your position every frame, but inertia decreases every frame
	UFUNCTION(BlueprintCallable)
	void SetInertia(int InInertia);
	//clears inertia
	UFUNCTION(BlueprintCallable)
	void ClearInertia();
	//enables inertia
	UFUNCTION(BlueprintCallable)
	void EnableInertia();
	//disables inertia
	UFUNCTION(BlueprintCallable)
	void DisableInertia();
	//halts momentum
	UFUNCTION(BlueprintCallable)
	void HaltMomentum();
	//expands pushbox width temporarily
	UFUNCTION(BlueprintCallable)
	void SetPushWidthExpand(int Expand);
	//sets direction
	UFUNCTION(BlueprintCallable)
	void SetFacing(bool NewFacingRight);
	//flips character
	UFUNCTION(BlueprintCallable)
	void FlipCharacter();
	//enables auto flip
	UFUNCTION(BlueprintCallable)
	void EnableFlip(bool Enabled);
	//enables hit
	UFUNCTION(BlueprintCallable)
	void EnableHit(bool Enabled);
	//sets attacking. while this is true, you can be counter hit, but you can hit the opponent and chain cancel.
	UFUNCTION(BlueprintCallable)
	void SetAttacking(bool Attacking);
	//gives the current move the head attribute. for use with air attacks
	UFUNCTION(BlueprintCallable)
	void SetHeadAttribute(bool HeadAttribute);
	//sets hit effect on normal hit
	UFUNCTION(BlueprintCallable)
	void SetHitEffect(FHitEffect InHitEffect);
	//sets hit effect on counter hit
	UFUNCTION(BlueprintCallable)
	void SetCounterHitEffect(FHitEffect InHitEffect);
	//creates common particle
	UFUNCTION(BlueprintCallable)
	void CreateCommonParticle(FString Name, EPosType PosType, FVector Offset = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator);
	//creates character particle
	UFUNCTION(BlueprintCallable)
	void CreateCharaParticle(FString Name, EPosType PosType, FVector Offset = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator);
	//creates character particle and attaches it to the object. only use with non-player objects.
	UFUNCTION(BlueprintCallable)
	void LinkCharaParticle(FString Name);
	//plays common sound
	UFUNCTION(BlueprintCallable)
	void PlayCommonSound(FString Name);
	//plays chara sound
	UFUNCTION(BlueprintCallable)
	void PlayCharaSound(FString Name);
	//attaches object to skeletal socket
	UFUNCTION(BlueprintCallable)
	void AttachToSocketOfObject(FString InSocketName, FVector Offset, EObjType ObjType);
	//detaches object from skeletal socket
	UFUNCTION(BlueprintCallable)
	void DetachFromSocket();
	//pauses round timer
	UFUNCTION(BlueprintCallable)
	void PauseRoundTimer(bool Pause);
	//sets object id
	UFUNCTION(BlueprintCallable)
	void SetObjectID(int InObjectID);
	//gets object by type
	UFUNCTION(BlueprintPure)
	ABattleActor* GetBattleActor(EObjType Type);
	//DO NOT USE ON PLAYERS. if object goes beyond screen bounds, deactivate
	UFUNCTION(BlueprintCallable)
	void DeactivateIfBeyondBounds();
	//DO NOT USE ON PLAYERS. sets the object to deactivate next frame.
	UFUNCTION(BlueprintCallable)
	void DeactivateObject();
	//views collision. only usable in development or debug builds
	UFUNCTION(BlueprintCallable)
	void CollisionView();
};
