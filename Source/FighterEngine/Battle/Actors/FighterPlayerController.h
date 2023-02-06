// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FighterPlayerController.generated.h"

class UInputMappingContext;
class ANetworkPawn;
/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API AFighterPlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	virtual void OnNetCleanup(UNetConnection* Connection) override;
public:
	void TrySettingFighterCameraToViewport();
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void SetupInputComponent() override;
	
	int Inputs;
	int Frame;

	UPROPERTY(EditAnywhere, Category="Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping;
	
	void PressUp();
	void ReleaseUp();
	void PressDown();
	void ReleaseDown();
	void PressLeft();
	void ReleaseLeft();
	void PressRight();
	void ReleaseRight();
	void PressL();
	void ReleaseL();
	void PressM();
	void ReleaseM();
	void PressH();
	void ReleaseH();
	void PressS();
	void ReleaseS();
	void PressA1();
	void ReleaseA1();
	void PressA2();
	void ReleaseA2();
	void PressDash();
	void ReleaseDash();

	void UpdateInput(int Input[], int32 InFrame, int32 InFrameAdvantage);
	void CheckForDesyncs(uint32 Checksum, int32 InFrame);
	void SendGgpo(ANetworkPawn* InNetworkPawn, bool Client);
	
	UFUNCTION(BlueprintCallable)
	void SendCharaData();

	UPROPERTY(BlueprintReadOnly)
	ANetworkPawn* NetworkPawn;
	ACameraActor* CurrentView=nullptr;
};	

