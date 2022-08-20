// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionData.h"
#include "ParticleData.h"
#include "StateDataAsset.h"
#include "BattleActor.generated.h"
#pragma pack (push, 1)

class UState;
class APlayerCharacter;

#define COORD_SCALE ((double)1000 / 0.43)

UENUM()
enum EPosType
{
	POS_Player,
	POS_Enemy,
	POS_Hit,
};

UENUM() 
enum EInternalValue //internal values list
{
	VAL_Angle,
	VAL_ActionFlag,
	VAL_PlayerVal0,
	VAL_PlayerVal1,
	VAL_PlayerVal2,
	VAL_PlayerVal3,
	VAL_SpeedX, 
	VAL_SpeedY,
	VAL_ActionTime,
	VAL_PosX,
	VAL_PosY,
	VAL_Inertia,
	VAL_FacingRight,
	VAL_Health,
	VAL_Hitstop,
};

UENUM()
enum HitAction
{
	HACT_None,
	HACT_GroundNormal,
	HACT_AirNormal,
};

UENUM()
enum EBlockType
{
	BLK_Mid,
	BLK_High,
	BLK_Low,
	BLK_None,
};

USTRUCT(BlueprintType)
struct FHitEffect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int AttackLevel; //in this engine, it's only used to define effects on hit or block
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EBlockType> BlockType;
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
	TEnumAsByte<HitAction> GroundHitAction;
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<HitAction> AirHitAction;
};

UCLASS()
class FIGHTERENGINE_API ABattleActor : public APawn
{
	GENERATED_BODY()
public:
	ABattleActor();
	
	unsigned char ObjSync; //starting from this until ObjSyncEnd, everything is saved/loaded for rollback
	bool IsActive = false;
protected:
	//internal values
	int PosX;
	int PosY;
	int PrevPosX;
	int PrevPosY;
	int SpeedX;
	int SpeedY;
	int Gravity = 1900;
	int Inertia;
	int ActionTime = -1;
	int PushHeight;
	int PushHeightLow;
	int PushWidth;
	int Hitstop;
	int L;
	int R;
	int T;
	int B;
	FHitEffect HitEffect;
	FHitEffect CounterHitEffect;
	bool HitActive;
	bool IsAttacking;
	bool RoundStart = true;
	bool HasHit;
	bool DeactivateOnNextUpdate;
	
public:	
	bool FacingRight;
	int MiscFlags;
	bool IsPlayer = false; //disabled if not player

	std::string CelNameInternal; //cel name copied from FString
	std::string HitEffectName; //for hit effect overrides
	
	//current animation time
	UPROPERTY(BlueprintReadWrite)
	int AnimTime = -1;

	//animbp time
	UPROPERTY(BlueprintReadWrite)
	int AnimBPTime = -1;

	//for spawning hit particles
	int HitPosX;
	int HitPosY;

	//for common actions: does it inherit default values?
	UPROPERTY(BlueprintReadWrite)
	bool DefaultCommonAction = true;

private:
	FCollisionBoxInternal CollisionBoxesInternal[0x20];
	
public:
	UPROPERTY()
	UState* ObjectState; //for use with non-character objects only. sets the state for it to use

	int ObjSyncEnd; //anything past here isn't saved or loaded for rollback

	UPROPERTY(EditAnywhere)
	UMaterial* BoxMaterial; //only used for rendering collision
	
	//current anim and hitbox
	UPROPERTY(BlueprintReadOnly)
	FString CelName; //cel name. used for animation and collision data calls.

private:
	
	UPROPERTY()
	TArray<FCollisionBoxInternal> CollisionBoxes; //grabbed collision boxes from collision data

public:

	//array of box data
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCollisionData* CollisionData; //collision data asset
	
	UPROPERTY()
	APlayerCharacter* Player; //pointer to player. if this is not a player, it will point to the owning player.
	
	UPROPERTY(BlueprintReadWrite)
	UNiagaraComponent* LinkedParticle; //non-player objects only. particle that moves with the object.
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Move(); //move object based on speed and inertia
	void GetBoxes(); //get boxes based on cel name

public:
	void SaveForRollback(unsigned char* Buffer);
	void LoadForRollback(unsigned char* Buffer);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//internal functions
	void HandlePushCollision(ABattleActor* OtherObj); //handles pushing objects
	void HandleHitCollision(APlayerCharacter* OtherChar); //handles hitting objects

	virtual void LogForSyncTest();

	void InitObject();
	virtual void Update();
	
	//bp callable functions
	UFUNCTION(BlueprintPure)
	int GetInternalValue(EInternalValue InternalValue); //gets internal value for bp
	UFUNCTION(BlueprintPure)
	bool IsOnFrame(int Frame); //checks if on frame
	UFUNCTION(BlueprintCallable)
	void SetCelName(FString InCelName); //sets cel name
	UFUNCTION(BlueprintCallable)
	void SetHitEffectName(FString InHitEffectName); //sets cel name
	UFUNCTION(BlueprintCallable)
	void SetPosX(int InPosX); //sets x position
	UFUNCTION(BlueprintCallable)
	void SetPosY(int InPosY); //sets y position
	UFUNCTION(BlueprintCallable)
	void AddPosX(int InPosX); //adds x position depending on direction
	UFUNCTION(BlueprintCallable)
	void AddPosXRaw(int InPosX); //adds x position with no regard for direction
	UFUNCTION(BlueprintCallable)
	void AddPosY(int InPosY); //adds y position
	UFUNCTION(BlueprintCallable)
	void SetSpeedX(int InSpeedX); //sets x speed
	UFUNCTION(BlueprintCallable)
	void SetSpeedY(int InSpeedY); //sets y speed
	UFUNCTION(BlueprintCallable)
	void AddSpeedX(int InSpeedX); //adds x speed
	UFUNCTION(BlueprintCallable)
	void AddSpeedY(int InSpeedY); //adds y speed
	UFUNCTION(BlueprintCallable)
	void SetGravity(int InGravity); //sets gravity
	UFUNCTION(BlueprintCallable)
	void SetInertia(int InInertia); //sets inertia. when inertia is enabled, inertia adds to your position every frame, but also decreases every frame
	UFUNCTION(BlueprintCallable)
	void ClearInertia(); //clears inertia
	UFUNCTION(BlueprintCallable)
	void EnableInertia(); //enables inertia
	UFUNCTION(BlueprintCallable)
	void DisableInertia(); //disables inertia
	UFUNCTION(BlueprintCallable)
	void HaltMomentum(); //halts momentum
	UFUNCTION(BlueprintCallable)
	void SetFacing(bool NewFacingRight); //sets direciton
	UFUNCTION(BlueprintCallable)
	void EnableFlip(bool Enabled); //enables flip
	UFUNCTION(BlueprintCallable)
	void EnableHit(bool Enabled); //enables hit
	UFUNCTION(BlueprintCallable)
	void SetAttacking(bool Attacking); //sets attacking. while this is true, you can be counter hit, but you can also chain cancel
	UFUNCTION(BlueprintCallable)
	void SetHitEffect(FHitEffect InHitEffect); //sets hit effect on normal hit
	UFUNCTION(BlueprintCallable)
	void SetCounterHitEffect(FHitEffect InHitEffect); //sets hit effect on counter hit
	UFUNCTION(BlueprintCallable)
	void CreateCommonParticle(FString Name, EPosType PosType, FVector Offset = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator); //creates common particle
	UFUNCTION(BlueprintCallable)
	void CreateCharaParticle(FString Name, EPosType PosType, FVector Offset = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator); //creates character particle
	UFUNCTION(BlueprintCallable)
	void LinkCharaParticle(FString Name); //creates character particle and attaches it to the object. only use with non-player objects.
	UFUNCTION(BlueprintCallable)
	void PlayCommonSound(FString Name);
	UFUNCTION(BlueprintCallable)
	void PlayCharaSound(FString Name);
	UFUNCTION(BlueprintCallable)
	void DeactivateObject(); //DO NOT USE ON PLAYERS. sets the object to deactivate next frame.
	void ResetObject();
	
	UFUNCTION(BlueprintCallable)
	void CollisionView(); //views collision
};
#pragma pack(pop)

#define SIZEOF_BATTLEACTOR offsetof(ABattleActor, ObjSyncEnd) - offsetof(ABattleActor, ObjSync)