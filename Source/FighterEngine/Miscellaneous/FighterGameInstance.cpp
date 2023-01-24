// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "FighterEngine/FighterGameModeBase.h"

bool UFighterGameInstance::Login()
{
	IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

	this->LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(
	0,
	FOnLoginComplete::FDelegate::CreateUObject(
		this,
		&UFighterGameInstance::HandleLoginComplete));
	if (!Identity->AutoLogin(0 /* LocalUserNum */))
	{
		return false;
	}
	return true;
}

void UFighterGameInstance::OnSessionInviteAccepted(bool bArg, int I,
	TSharedPtr<const FUniqueNetId, ESPMode::ThreadSafe> UniqueNetId,
	const FOnlineSessionSearchResult& OnlineSessionSearchResult)
{
	InviteResult = OnlineSessionSearchResult;
	
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	this->JoinSessionDelegateHandle =
		Session->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionComplete::FDelegate::CreateUObject(
				this,
				&UFighterGameInstance::HandleJoinInviteSessionComplete));

	// "MyLocalSessionName" is the local name of the session for this player. It doesn't have to match the name the server gave their session.
	Session->JoinSession(0, FName(TEXT("MyLocalSessionName")), OnlineSessionSearchResult);
}

void UFighterGameInstance::HandleLoginComplete(int I, bool bArg, const FUniqueNetId& UniqueNetId, const FString& String)
{
	IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	Identity->ClearOnLoginCompleteDelegate_Handle(0, this->LoginDelegateHandle);
	this->LoginDelegateHandle.Reset();
}

bool UFighterGameInstance::GetFriendsList()
{
	IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();
	
	return Friends->ReadFriendsList(
	0 /* LocalUserNum */,
	TEXT("") /* ListName, unused by EOS */,
	FOnReadFriendsListComplete::CreateUObject(this, &UFighterGameInstance::OnReadComplete)
	);
}

bool UFighterGameInstance::CreateSession()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	
	this->CreateSessionDelegateHandle =
		Session->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionComplete::FDelegate::CreateUObject(
			this,
			&UFighterGameInstance::HandleCreateSessionComplete));
	TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();

	SessionSettings->NumPublicConnections = 2; // The number of players.
	SessionSettings->bShouldAdvertise = true;  // Set to true to make this session discoverable with FindSessions.
	SessionSettings->bUsesPresence = true;    // Set to true if you want this session to be discoverable by presence (Epic Social Overlay).
	SessionSettings->bAllowInvites = true;    // Set to true if you want this session to be allow invites.

	// You *must* set at least one setting value, because you can not run FindSessions without any filters.
	SessionSettings->Settings.Add(
		FName(TEXT("Version")),
		FOnlineSessionSetting(static_cast<int64>(FNetworkVersion::GetLocalNetworkVersion()), EOnlineDataAdvertisementType::ViaOnlineService));

	// Create a session and give the local name "MyLocalSessionName". This name is entirely local to the current player and isn't stored in EOS.
	if (!Session->CreateSession(0, FName(TEXT("MyLocalSessionName")), *SessionSettings))
	{
		return false;
	}
	return true;
}

void UFighterGameInstance::OnReadComplete(int I, bool bArg, const FString& String, const FString& String1)
{
	FriendInfos.Empty();
	
	IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();
	
	TArray<TSharedRef<FOnlineFriend>> FriendsArr;
	Friends->GetFriendsList(
		0 /* LocalUserNum */,
		TEXT("") /* ListName, unused by EOS */,
		FriendsArr /* OutFriends */
	);
	for (const auto& Friend : FriendsArr)
	{
		FFriendInfo FriendInfo;
		FriendInfo.DisplayName = Friend->GetDisplayName();
		FriendInfo.NetId = Friend->GetUserId();
		FriendInfos.Add(FriendInfo);
	}
}

bool UFighterGameInstance::SearchForServer()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	
	TSharedRef<FOnlineSessionSearch> Search = MakeShared<FOnlineSessionSearch>();
	// Remove the default search parameters that FOnlineSessionSearch sets up.
	Search->QuerySettings.SearchParams.Empty();
	
	Search->QuerySettings.SearchParams.Add(
		FName(TEXT("Version")), 
		FOnlineSessionSearchParam(
			static_cast<int64>(FNetworkVersion::GetLocalNetworkVersion()),
			EOnlineComparisonOp::Equals));

	this->FindSessionsDelegateHandle =
	Session->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsComplete::FDelegate::CreateUObject(
		this,
		&UFighterGameInstance::HandleFindSessionsComplete,
		Search));
	
	if (!Session->FindSessions(0, Search))
	{
		return false;
	}
	
	return true;
}

void UFighterGameInstance::HandleFindSessionsComplete(bool bArg, TSharedRef<FOnlineSessionSearch, ESPMode::ThreadSafe> Shared)
{
	IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	if (bArg)
	{
		for (auto RawResult : Shared->SearchResults)
		{
			if (RawResult.IsValid())
			{
				FString ConnectInfo;
				if (Session->GetResolvedConnectString(RawResult, NAME_GamePort, ConnectInfo))
				{
					FSessionInfo SessionInfo;
					SessionInfo.Result = RawResult;
					SessionInfo.ConnectInfo = ConnectInfo;

					bool Duplicate = false;
					
					for (auto StoredSession : SessionInfos)
					{
						if (StoredSession.ConnectInfo == ConnectInfo)
						{
							Duplicate = true;
							break;
						}
					}

					if (!Duplicate)
						SessionInfos.Add(SessionInfo);
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Session count: %d"), SessionInfos.Num())
	
	Session->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionsDelegateHandle);
	this->FindSessionsDelegateHandle.Reset();
}

bool UFighterGameInstance::JoinServer(FString ConnectInfo)
{
	FOnlineSessionSearchResult SearchResult;

	for (int i = 0; i < SessionInfos.Num(); i++)
	{
		if (ConnectInfo == SessionInfos[i].ConnectInfo)
		{
			SearchResult = SessionInfos[i].Result;
			SessionIndex = i;
			break;
		}
		if (i == i < SessionInfos.Num() - 1)
			return false;
	}
	
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	this->JoinSessionDelegateHandle =
		Session->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionComplete::FDelegate::CreateUObject(
				this,
				&UFighterGameInstance::HandleJoinSessionComplete));

		// "MyLocalSessionName" is the local name of the session for this player. It doesn't have to match the name the server gave their session.
	if (!Session->JoinSession(0, FName(TEXT("MyLocalSessionName")), SearchResult))
	{
		return false;
	}
	return true;
}

bool UFighterGameInstance::DestroySession()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	this->DestroySessionDelegateHandle =
	Session->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionComplete::FDelegate::CreateUObject(
			this,
			&UFighterGameInstance::HandleDestroySessionComplete));

	if (!Session->DestroySession(FName(TEXT("MyLocalSessionName"))))
	{
		return false;
	}
	return true;
}

void UFighterGameInstance::SeamlessTravel()
{
	this->GetWorld()->ServerTravel(StageURL, true);
}

void UFighterGameInstance::HandleJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Arg)
{
	if (Arg == EOnJoinSessionCompleteResult::Success ||
		Arg == EOnJoinSessionCompleteResult::AlreadyInSession)
	{
		if (GEngine != nullptr)
		{
			FURL NewURL(nullptr, *SessionInfos[SessionIndex].ConnectInfo, ETravelType::TRAVEL_Absolute);
			FString BrowseError;
			if (GEngine->Browse(GEngine->GetWorldContextFromWorldChecked(this->GetWorld()), NewURL, BrowseError) ==
				EBrowseReturnVal::Failure)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to start browse: %s"), *BrowseError);
			}
		}
		// Use the connection string that you got from FindSessions in order
		// to connect to the server.
		//
		// Refer to "Connecting to a game server" under the "Networking & Anti-Cheat"
		// section of the documentation for more information on how to do this.
		//
		// NOTE: You can also call GetResolvedConnectString at this point instead
		// of in FindSessions, but it's recommended that you call it in
		// FindSessions so you know the result is valid.
	}

	IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	Session->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
	this->JoinSessionDelegateHandle.Reset();
}

void UFighterGameInstance::HandleJoinInviteSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Arg)
{
	IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	if (Arg == EOnJoinSessionCompleteResult::Success ||
		Arg == EOnJoinSessionCompleteResult::AlreadyInSession)
	{
		if (GEngine != nullptr)
		{
			FString ConnectInfo;
			if (Session->GetResolvedConnectString(InviteResult, NAME_GamePort, ConnectInfo))
			{
				FSessionInfo SessionInfo;
				SessionInfo.Result = InviteResult;
				SessionInfo.ConnectInfo = ConnectInfo;

				FURL NewURL(nullptr, *SessionInfo.ConnectInfo, ETravelType::TRAVEL_Absolute);
				FString BrowseError;
				if (GEngine->Browse(GEngine->GetWorldContextFromWorldChecked(this->GetWorld()), NewURL, BrowseError) ==
					EBrowseReturnVal::Failure)
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to start browse: %s"), *BrowseError);
				}
			}
		}
		// Use the connection string that you got from FindSessions in order
		// to connect to the server.
		//
		// Refer to "Connecting to a game server" under the "Networking & Anti-Cheat"
		// section of the documentation for more information on how to do this.
		//
		// NOTE: You can also call GetResolvedConnectString at this point instead
		// of in FindSessions, but it's recommended that you call it in
		// FindSessions so you know the result is valid.
	}

	Session->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
	this->JoinSessionDelegateHandle.Reset();
}

void UFighterGameInstance::HandleDestroySessionComplete(FName Name, bool bArg)
{
	IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	Session->ClearOnDestroySessionCompleteDelegate_Handle(this->DestroySessionDelegateHandle);
	this->DestroySessionDelegateHandle.Reset();
}

void UFighterGameInstance::HandleCreateSessionComplete(FName Name, bool bArg)
{
	IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	Session->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
	this->CreateSessionDelegateHandle.Reset();

	AFighterGameModeBase* GameMode = Cast<AFighterGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
		GameMode->RegisterExistingPlayers();

	Session->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UFighterGameInstance::OnSessionInviteAccepted);
}


