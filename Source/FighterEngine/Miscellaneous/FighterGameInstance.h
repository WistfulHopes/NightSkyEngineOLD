// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FighterRunners.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FighterGameInstance.generated.h"


USTRUCT(BlueprintType)
struct FFriendInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;
	FUniqueNetIdPtr NetId;
};

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	FOnlineSessionSearchResult Result;
	UPROPERTY(BlueprintReadOnly)
	FString ConnectInfo;
};

class APlayerCharacter;
enum class ERoundFormat : uint8;
/**
 * 
 */
UCLASS()
class FIGHTERENGINE_API UFighterGameInstance : public UGameInstance
{
	GENERATED_BODY()

	FDelegateHandle LoginDelegateHandle;
	FDelegateHandle FindSessionsDelegateHandle;
	FDelegateHandle JoinSessionDelegateHandle;
	FDelegateHandle DestroySessionDelegateHandle;
	FDelegateHandle CreateSessionDelegateHandle;
	int SessionIndex;

	void OnSessionInviteAccepted(bool bArg, int I, TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UniqueNetId, const FOnlineSessionSearchResult& OnlineSessionSearchResult);
	void HandleLoginComplete(int I, bool bArg, const FUniqueNetId& UniqueNetId, const FString& String);
	void OnReadComplete(int I, bool bArg, const FString& String, const FString& String1);
	void HandleFindSessionsComplete(bool bArg, TSharedRef<FOnlineSessionSearch, ESPMode::ThreadSafe> Shared);
	void HandleJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Arg);
	void HandleJoinInviteSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Arg);
	void HandleDestroySessionComplete(FName Name, bool bArg);
	void HandleCreateSessionComplete(FName Name, bool bArg);
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<APlayerCharacter>> PlayerList;
	UPROPERTY(BlueprintReadWrite)
	int PlayerIndex;
	UPROPERTY(BlueprintReadWrite)
	FString StageURL;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ERoundFormat RoundFormat;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int StartRoundTimer;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool IsTraining = false;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TEnumAsByte<EFighterRunners> FighterRunner;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<ACameraActor> FighterCameraActor;
	
	UPROPERTY(BlueprintReadOnly)
    TArray<FFriendInfo> FriendInfos;
    UPROPERTY(BlueprintReadOnly)
    TArray<FSessionInfo> SessionInfos;

	FOnlineSessionSearchResult InviteResult;

	UFUNCTION(BlueprintCallable)
	bool Login();
	UFUNCTION(BlueprintCallable)
	bool GetFriendsList();
	UFUNCTION(BlueprintCallable)
	bool CreateSession();
	UFUNCTION(BlueprintCallable)
	bool SearchForServer();
	UFUNCTION(BlueprintCallable)
	bool JoinServer(FString ConnectInfo);
	UFUNCTION(BlueprintCallable)
	bool DestroySession();
	UFUNCTION(BlueprintCallable)
	void SeamlessTravel();
};
