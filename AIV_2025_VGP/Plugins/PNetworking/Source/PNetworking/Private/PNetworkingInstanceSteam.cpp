// Copyright Epic Games, Inc. All Rights Reserved.

// • Manuel Solano
// • Alessandro Caccamo
// • Claudio Dallai

#include "PNetworkingInstanceSteam.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSubsystem.h"
#include "UserSteamData.h"
#include "PNetworking.h"
#include "Kismet/GameplayStatics.h"
#include "SessionCreationParameters.h"

// Static declarations.
UPNetworkingInstanceSteam* UPNetworkingInstanceSteam::NetInstanceSteamPtr = nullptr;
bool UPNetworkingInstanceSteam::bIsModuleReady = false;

#pragma region SpecialMemberFunctions

UPNetworkingInstanceSteam::UPNetworkingInstanceSteam()
{
	UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("Constructor UPNetworkingInstanceSteam Called!"));
	if (bIsModuleReady)
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("Module is ready. Calling InitializeNetworkingInstance!"));
		InitializeNetworkingInstance();
	}
	else
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("Module is not ready. Not calling InitializeNetworkingInstance!"));
	}
}

UPNetworkingInstanceSteam::~UPNetworkingInstanceSteam()
{
}

#pragma endregion SpecialMemberFunctions

#pragma region SingletonePluginManagement

UPNetworkingInstanceSteam* UPNetworkingInstanceSteam::GetUniqueInstance()
{
	if (!bIsModuleReady)
	{
		// The moment this function can be called in blueprint, the plugin, modules, OSS could be taken correctly.
		// Set relative dirty flag to true, and proceed to initialize NetworkingInstance inside the constructor.
		bIsModuleReady = true;
	}

	if (NetInstanceSteamPtr == nullptr)
	{
		NetInstanceSteamPtr = NewObject<UPNetworkingInstanceSteam>();
		if (NetInstanceSteamPtr) 
		{
			UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("GetUniqueInstance: Building unique instance of NET Steam manager"));
			NetInstanceSteamPtr->AddToRoot();
		}
	}
	else
	{
		UE_LOG(LogSteamNetworkingPlugin, Display, TEXT("GetUniqueInstance: Requested already valid unique instance of NET Steam manager"));
	}

	return NetInstanceSteamPtr;
}

void UPNetworkingInstanceSteam::DeleteUniqueInstance()
{
	if (NetInstanceSteamPtr != nullptr)
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("DeleteUniqueInstance: Removing from root NET Steam manager and invaliding ptr"));
		NetInstanceSteamPtr->DeInitializeNetworkingInstance();
		NetInstanceSteamPtr->RemoveFromRoot();
		NetInstanceSteamPtr = nullptr;
		bIsModuleReady = false;
	}
	else
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("DeleteUniqueInstance: NET Steam manager pointer was already invalid"));
	}
}

#pragma endregion SingletonePluginManagement

#pragma region LocalUser

bool UPNetworkingInstanceSteam::GetAppID(FString& AppID)
{
	IOnlineSubsystem* OnlineSubsystemPtr = FPNetworkingModule::GetOnlineSubsystemPointer();

	if (!OnlineSubsystemPtr)
	{
		return false;
	}

	AppID = OnlineSubsystemPtr->GetAppId();

	return true;
}

bool UPNetworkingInstanceSteam::GetLocalCSteamID(int32& OutSteamID)
{
	ISteamUser* SteamUserInterface = SteamUser();
	if (!SteamUserInterface)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetLocalCSteamID: SteamUserInterface steamwork_sdk not valid!"));
		return false;
	}

	const CSteamID RequestedSteamID = SteamUserInterface->GetSteamID();
	if (!RequestedSteamID.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetLocalCSteamID: Obtained CSteamID not valid!"));
		return false;
	}

	OutSteamID = static_cast<int32>(RequestedSteamID.GetAccountID());
	return true;
}

bool UPNetworkingInstanceSteam::GetAccountName(FString& AccountName, const int32 UserID)
{
	IOnlineSubsystem* OnlineSubsystemPtr = FPNetworkingModule::GetOnlineSubsystemPointer();

	if (!OnlineSubsystemPtr)
	{
		return false;
	}

	IOnlineIdentityPtr OnlineIdentityPtr = OnlineSubsystemPtr->GetIdentityInterface();
	if (!OnlineIdentityPtr.IsValid())
	{
		return false;
	}

	FUniqueNetIdPtr UniqueNetId = OnlineIdentityPtr->GetUniquePlayerId(UserID); // Local Player (client).

	if (!UniqueNetId.IsValid())
	{
		return false;
	}

	AccountName = OnlineIdentityPtr->GetPlayerNickname(*UniqueNetId);
	return true;
}

int32 UPNetworkingInstanceSteam::GetLocalUserAvatar(const FOnLocalAvatarReady& Callback)
{
	if (!FPNetworkingModule::IsOnlineAvailable(TEXT("IsOnlineAvailable: GetLocalUserAvatar Called it")))
	{
		return 0;
	}

	return GetLocalUserAvatarRecursive(MakeShared<FOnLocalAvatarReady>(Callback));
}

#pragma endregion LocalUser

#pragma region FriendlistUtilityLocalUser

FString UPNetworkingInstanceSteam::GetUsernameFromSteamID(const int32 SteamID)
{
	if (!FPNetworkingModule::IsOnlineAvailable(TEXT("IsOnlineAvailable: GetUsernameFromSteamID Called it")))
	{
		return EMPTY_FSTRING;
	}

	const CSteamID RealSteamID = ConvertInt32toCSteamID(SteamID);
	ISteamFriends* SteamFriendsInterface = SteamFriends();
	if (!SteamFriendsInterface)
	{
		return EMPTY_FSTRING;
	}

	return FString(SteamFriendsInterface->GetFriendPersonaName(RealSteamID));
}

int32 UPNetworkingInstanceSteam::GetAvatarFromSteamID(const int32 SteamID, const FOnRequestedFriendAvatarReady& Callback)
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return 0;
	}

	CSteamID TargetFriendID = ConvertInt32toCSteamID(SteamID);
	if (!TargetFriendID.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetAvatarFromSteamID: CSteamID not valid!"));
		return 0;
	}

	return GetRequestedFriendAvatarRecursive(TargetFriendID, MakeShared<FOnRequestedFriendAvatarReady>(Callback));
}

bool UPNetworkingInstanceSteam::GetOnlineFriendListNames(const FOnFriendsListReady& Callback, const int32 LocalUserNum)
{
	return GetFriendList(Callback, EFriendsLists::OnlinePlayers, LocalUserNum);
}

bool UPNetworkingInstanceSteam::GetAllFriendListNames(const FOnFriendsListReady& Callback, const int32 LocalUserNum)
{
	return GetFriendList(Callback, EFriendsLists::Default, LocalUserNum);
}

int32 UPNetworkingInstanceSteam::GetFriendsAvatar(const FOnFriendsAvatarReady& Callback)
{
	if (!FPNetworkingModule::IsOnlineAvailable(TEXT("IsOnlineAvailable: GetFriendsAvatar Called it")))
	{
		return 0;
	}

	return GetFriendsAvatarRecursive(MakeShared<FOnFriendsAvatarReady>(Callback));
}

int32 UPNetworkingInstanceSteam::GetPlayersData(const bool bAlphabeticalSort, const FOnFriendsDataReady& Callback)
{
	if (!FPNetworkingModule::IsOnlineAvailable(TEXT("IsOnlineAvailable: GetPlayersData Called it")))
	{
		return 0;
	}

	return GetPlayerDataRecursive(bAlphabeticalSort, MakeShared<FOnFriendsDataReady>(Callback));
} 

#pragma endregion FriendlistUtilityLocalUser

#pragma region SessionManagement

bool UPNetworkingInstanceSteam::RequestSessionCreation(FSessionCreationParameters SessionCreationParameters)
{
	if (FPNetworkingModule::GetLocalSessionCurrentState() != ELocalSessionState::SESSION_INVALID)
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("RequestSessionCreation: Already computing a session!"));
		return false;
	}

	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionPointer();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("RequestSessionCreation: Session interface not valid!"));
		return false;
	}

	FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_PENDING);

	MapPathToTravel = SessionCreationParameters.TravelToMapPath;
	TempCreationSessionSettings.NumPublicConnections = SessionCreationParameters.NumPublicConnections;
	TempCreationSessionSettings.NumPrivateConnections = SessionCreationParameters.NumPrivateConnections;
	TempCreationSessionSettings.bShouldAdvertise = SessionCreationParameters.bShouldAdvertise;
	TempCreationSessionSettings.bAllowJoinInProgress = SessionCreationParameters.bAllowJoinInProgress;
	TempCreationSessionSettings.bIsLANMatch = SessionCreationParameters.bIsLANMatch;
	TempCreationSessionSettings.bIsDedicated = SessionCreationParameters.bIsDedicated;
	TempCreationSessionSettings.bAllowInvites = SessionCreationParameters.bAllowInvites;
	TempCreationSessionSettings.bUsesPresence = SessionCreationParameters.bUsesPresence;
	TempCreationSessionSettings.bAllowJoinViaPresence = SessionCreationParameters.bAllowJoinViaPresence;
	TempCreationSessionSettings.bAllowJoinViaPresenceFriendsOnly = SessionCreationParameters.bAllowJoinViaPresenceFriendsOnly;
	TempCreationSessionSettings.bUseLobbiesIfAvailable = SessionCreationParameters.bUseLobbiesIfAvailable;

	return HandleOldSessionIfExisting();
}

bool UPNetworkingInstanceSteam::InviteFriend(const int32 SteamID)
{
	if (FPNetworkingModule::GetLocalSessionCurrentState() != ELocalSessionState::SESSION_VALID)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("InviteFriend: Session is begin computed, destroyed or already invalid!"));
		return false;
	}

	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionPointer();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("InviteFriend: SessionInterface is invalid!"));
		return false;
	}

	const CSteamID FriendSteamID = ConvertInt32toCSteamID(SteamID);
	FUniqueNetIdPtr FriendUniqueNetID;

	if (ConvertCSteamIDToFUniqueNetID(FriendSteamID, FriendUniqueNetID))
	{
		if (!SessionInterface->GetNamedSession(FPNetworkingModule::GetSessionName()))
		{
			UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("InviteFriend: Session was not created!"));
			return false;
		}

		return SessionInterface->SendSessionInviteToFriend(0, FPNetworkingModule::GetSessionName(), *FriendUniqueNetID);
	}

	return false;
}

void UPNetworkingInstanceSteam::QuitSession(const FString& TravelBackMapPath)
{
	if (FPNetworkingModule::GetLocalSessionCurrentState() != ELocalSessionState::SESSION_VALID)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("QuitSession: Session is begin computed, destroyed or already invalid!"));
		return;
	}

	if (!GEngine)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("QuitSession: GEngine invalid!"));
		return;
	}

	UWorld* CurrentWorld = GEngine->GetWorldContexts().Num() > 0 ? GEngine->GetWorldContexts()[0].World() : nullptr;
	if (!CurrentWorld)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("QuitSession: World is null!"));
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(CurrentWorld, 0);
	if (!PlayerController)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("QuitSession: PlayerController is null!"));
		return;
	}
	
	PlayerController->ClientTravel(TravelBackMapPath, ETravelType::TRAVEL_Absolute);

	IOnlineSessionPtr OnlineSession = FPNetworkingModule::GetOnlineSessionPointer();
	if (!OnlineSession.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("QuitSession: OnlineSession is null!"));
		return;
	}

	if (OnlineSession->GetNamedSession(FPNetworkingModule::GetSessionName()))
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("QuitSession: Session existing, need to destroy it!"));
		OnClientDestroySessionCompleteHandle = OnlineSession->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnClientDestroySessionComplete));

		if (OnlineSession->DestroySession(FPNetworkingModule::GetSessionName()))
		{
			FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_DESTROYING);
			UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("QuitSession: DestroySession request true!"));
		}
		else
		{
			if (OnClientDestroySessionCompleteHandle.IsValid())
			{
				OnlineSession->ClearOnDestroySessionCompleteDelegate_Handle(OnClientDestroySessionCompleteHandle);
				OnClientDestroySessionCompleteHandle.Reset();
			}
		}
	}
}

bool UPNetworkingInstanceSteam::GetSessionParameters(FGetSessionParameters& SessionParameters) const
{
	IOnlineSessionPtr OnlineSession = FPNetworkingModule::GetOnlineSessionPointer();
	if (!OnlineSession.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetSessionParameters: OnlineSession is null!"));
		return false;
	}

	FOnlineSessionSettings* SessionSettings = OnlineSession->GetSessionSettings(FPNetworkingModule::GetSessionName());
	if (!SessionSettings)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetSessionParameters: SessionSettings is null!"));
		return false;
	}
	
	SessionParameters.NumPublicConnections = SessionSettings->NumPublicConnections;
	SessionParameters.NumPrivateConnections = SessionSettings->NumPrivateConnections;
	SessionParameters.bShouldAdvertise = SessionSettings->bShouldAdvertise;
	SessionParameters.bAllowJoinInProgress = SessionSettings->bAllowJoinInProgress;
	SessionParameters.bIsLANMatch = SessionSettings->bIsLANMatch;
	SessionParameters.bIsDedicated = SessionSettings->bIsDedicated;
	SessionParameters.bAllowInvites = SessionSettings->bAllowInvites;

	return true;
}

bool UPNetworkingInstanceSteam::UpdateSessionParameters_AuthorityOnly(const FUpdateSessionParameters& SessionParameters, const FOnSessionParametersUpdateReady& Callback)
{
	IOnlineSessionPtr OnlineSession = FPNetworkingModule::GetOnlineSessionPointer();
	if (!OnlineSession.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("UpdateSessionParameters_AuthorityOnly: OnlineSession is null!"));
		return false;
	}

	FOnlineSessionSettings* SessionSettings = OnlineSession->GetSessionSettings(FPNetworkingModule::GetSessionName());
	if (!SessionSettings)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("UpdateSessionParameters_AuthorityOnly: SessionSettings is null!"));
		return false;
	}

	SessionSettings->NumPublicConnections = SessionParameters.NumPublicConnections;
	SessionSettings->NumPrivateConnections = SessionParameters.NumPrivateConnections;
	SessionSettings->bShouldAdvertise = SessionParameters.bShouldAdvertise;
	SessionSettings->bAllowJoinInProgress = SessionParameters.bAllowJoinInProgress;
	SessionSettings->bIsLANMatch = SessionParameters.bIsLANMatch;
	SessionSettings->bIsDedicated = SessionParameters.bIsDedicated;
	SessionSettings->bAllowInvites = SessionParameters.bAllowInvites;

	OnSessionParametersUpdateReadyDelegateHandle = OnlineSession->AddOnUpdateSessionCompleteDelegate_Handle(FOnUpdateSessionCompleteDelegate::CreateLambda([this, Callback, OnlineSession](FName SessionName, bool bWasSuccessful) mutable
	{
		if (!bWasSuccessful)
		{
			return;
		}
		
		Callback.ExecuteIfBound(SessionName, bWasSuccessful);

		if (!OnlineSession.IsValid())
		{
			return;
		}

		if (OnSessionParametersUpdateReadyDelegateHandle.IsValid())
		{
			OnlineSession->ClearOnUpdateSessionCompleteDelegate_Handle(OnSessionParametersUpdateReadyDelegateHandle);
			OnSessionParametersUpdateReadyDelegateHandle.Reset();
		}
		
	}));
	
	OnlineSession->UpdateSession(FPNetworkingModule::GetSessionName(), *SessionSettings);

	return true;
}

bool UPNetworkingInstanceSteam::IsSessionJoinable() const
{
	FGetSessionParameters GotSessionParameters;
	
	if (!GetSessionParameters(GotSessionParameters))
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("IsSessionJoinable: Cannot retreive parameters!"));
		return false;
	}

	return  FPNetworkingModule::GetLocalSessionCurrentState() == ELocalSessionState::SESSION_VALID &&
		    GotSessionParameters.bShouldAdvertise &&
		    GotSessionParameters.bAllowJoinInProgress &&
		    GotSessionParameters.bAllowInvites;
}

#pragma endregion SessionManagement

#pragma region PrivateUtilityFunctions

int32 UPNetworkingInstanceSteam::GetOnlineFriendsFromFriendCount(const int32 FriendsCount)
{
	int32 OnlineFriends = 0;

	if (!FPNetworkingModule::IsOnlineAvailable(TEXT("IsOnlineAvailable: GetOnlineFriendsFromFriendCount Called it")))
	{
		return OnlineFriends;
	}

	for (int32 Index = 0; Index < FriendsCount; Index++)
	{
		const CSteamID CurrentSteamID = SteamFriends()->GetFriendByIndex(Index, k_EFriendFlagImmediate);
		//UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("GetOnlineFriendsFromFriendCount: ID: %u, NAME: %hs"), CurrentSteamID.GetAccountID(), SteamFriends()->GetFriendPersonaName(CurrentSteamID));
		const EPersonaState CurrentPlayerState = SteamFriends()->GetFriendPersonaState(CurrentSteamID);
		if (CurrentPlayerState != EPersonaState::k_EPersonaStateOffline)
		{
			OnlineFriends++;
		}
	}

	return OnlineFriends;
}

bool UPNetworkingInstanceSteam::GetFriendList(const FOnFriendsListReady& Callback, const EFriendsLists::Type Query, const int32 LocalUserNum)
{
	IOnlineSubsystem* OnlineSubsystemPtr = FPNetworkingModule::GetOnlineSubsystemPointer();
	if (!OnlineSubsystemPtr)
	{
		return false;
	}

	IOnlineFriendsPtr FriendsInterface = OnlineSubsystemPtr->GetFriendsInterface();
	if (!FriendsInterface.IsValid())
	{
		return false;
	}

	// Read the friends list before get all the names.
	const bool bGetFriendsSuccessful = FriendsInterface->ReadFriendsList(
		LocalUserNum,
		EFriendsLists::ToString(Query),
		FOnReadFriendsListComplete::CreateLambda([Callback](int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr) mutable
			{
				TArray<FString> FriendsListNames;

				if (!bWasSuccessful)
				{
					return;
				}

				IOnlineSubsystem* OnlineSubsystemPtr = FPNetworkingModule::GetOnlineSubsystemPointer();
				if (!OnlineSubsystemPtr)
				{
					return;
				}

				IOnlineFriendsPtr OnlineFriendsPtr = OnlineSubsystemPtr->GetFriendsInterface();
				if (!OnlineFriendsPtr.IsValid())
				{
					return;
				}

				TArray<TSharedRef<FOnlineFriend>> FriendsList;
				OnlineFriendsPtr->GetFriendsList(LocalUserNum, ListName, FriendsList); // Getting all friends.

				for (const TSharedRef<FOnlineFriend>& Friend : FriendsList)
				{
					FriendsListNames.Add(Friend->GetDisplayName()); // Adding all friend's names.
				}

				Callback.ExecuteIfBound(FriendsListNames); // Invoking the delegate: the function is ready!
			}));

	return bGetFriendsSuccessful;
}

void UPNetworkingInstanceSteam::AlphabeticalSortFriends(TArray<FUserSteamData>& FriendsToSort)
{
	FriendsToSort.Sort([](const FUserSteamData& First, const FUserSteamData& Second) { return First.UserName.ToString() < Second.UserName.ToString(); });
}

bool UPNetworkingInstanceSteam::ConvertCSteamIDToFUniqueNetID(const CSteamID SteamID, FUniqueNetIdPtr& CorrespondanceNetID)
{
	IOnlineSubsystem* OnlineSubsystemPtr = FPNetworkingModule::GetOnlineSubsystemPointer();

	if (!OnlineSubsystemPtr)
	{
		return false;
	}

	uint64 SteamID64 = SteamID.ConvertToUint64();
	FString SteamIDString = FString::Printf(TEXT("%llu"), SteamID64);

	IOnlineIdentityPtr IdentityInterface = OnlineSubsystemPtr->GetIdentityInterface();
	if (!IdentityInterface.IsValid())
	{
		return false;
	}

	FUniqueNetIdPtr UniqueNetId = IdentityInterface->CreateUniquePlayerId(SteamIDString);
	if (UniqueNetId.IsValid())
	{
		CorrespondanceNetID = UniqueNetId;
		return true;
	}

	return false;
}

CSteamID UPNetworkingInstanceSteam::ConvertInt32toCSteamID(const int32 SteamID)
{
	if (!FPNetworkingModule::IsOnlineAvailable(TEXT("IsOnlineAvailable: ConvertInt32toCSteamID Called it")))
	{
		return CSteamID();
	}

	const uint32 UnsignedSteamID = static_cast<uint32>(SteamID);
	CSteamID RealSteamID(UnsignedSteamID, EUniverse::k_EUniversePublic, EAccountType::k_EAccountTypeIndividual);
	return RealSteamID;
}

bool UPNetworkingInstanceSteam::HandleOldSessionIfExisting()
{
	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionPointer();
	if (!SessionInterface)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("HandleOldSessionIfExisting: SessionInterface is invalid!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return false;
	}

	FNamedOnlineSession* ExistingSession = FPNetworkingModule::GetOnlineSessionPointer()->GetNamedSession(FPNetworkingModule::GetSessionName());
	if (ExistingSession)
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("HandleOldSessionIfExisting: Old session found!"));
		DestroySession();
	}
	else
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("HandleOldSessionIfExisting: Old session not found!"));
		CreateSession();
	}

	return true;
}

void UPNetworkingInstanceSteam::DestroySession()
{
	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionPointer();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("DestroySession: SessionInterface is invalid!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_DESTROYING);

	OnDestroySessionCompleteFromNewHostingUserHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
		FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnDestroySessionCompleteFromNewHostingUser));

	if (OnDestroySessionCompleteFromNewHostingUserHandle.IsValid() && SessionInterface->DestroySession(FPNetworkingModule::GetSessionName()))
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("DestroySession: Old session from new Host destroy call successfull!"));
	}
	else
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("DestroySession: Can't Destroy old session or not existing!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		if (OnDestroySessionCompleteFromNewHostingUserHandle.IsValid())
		{
			SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteFromNewHostingUserHandle);
			OnDestroySessionCompleteFromNewHostingUserHandle.Reset();
		}
	}
}

void UPNetworkingInstanceSteam::CreateSession()
{
	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionPointer();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("CreateSession: SessionInterface is invalid!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnCreateSessionComplete));

	SessionInterface->CreateSession(0, FPNetworkingModule::GetSessionName(), TempCreationSessionSettings);
}

bool UPNetworkingInstanceSteam::InitializeNetworkingInstance()
{
	if (!FPNetworkingModule::IsOnlineAvailable(TEXT("IsOnlineAvailable: InitializeNetworkingInstance Called it")))
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("InitializeNetworkingInstance: Online not available!"));
		return false;
	}

	FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);

	SessionUserInviteAcceptedDelegateHandle = FPNetworkingModule::GetOnlineSessionPointer()->AddOnSessionUserInviteAcceptedDelegate_Handle(
		FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnInviteAccepted));

	if (SessionUserInviteAcceptedDelegateHandle.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("InitializeNetworkingInstance: AcceptInvite Callback Initialized!"));
	}
	else
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("InitializeNetworkingInstance: AcceptInvite Callback not initialized!"));
		return false;
	}

	if (GEngine)
	{
		OnNetworkFailureDelegateHandle = GEngine->OnNetworkFailure().AddUObject(this, &UPNetworkingInstanceSteam::OnNetworkFailure);

		if (OnNetworkFailureDelegateHandle.IsValid())
		{
			UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("InitializeNetworkingInstance: Network failure delegate registered!"));
		}
		else
		{
			UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("InitializeNetworkingInstance: Network failure delegate not registered!"));
			return false;
		}
	}
	else
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("InitializeNetworkingInstance: GEngine is invalid!"));
		return false;
	}

	return true;
}

void UPNetworkingInstanceSteam::DeInitializeNetworkingInstance()
{
	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionPointer();
	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (SessionUserInviteAcceptedDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnSessionUserInviteAcceptedDelegate_Handle(SessionUserInviteAcceptedDelegateHandle);
		SessionUserInviteAcceptedDelegateHandle.Reset();
	}

	if (GEngine)
	{
		if (OnNetworkFailureDelegateHandle.IsValid())
		{
			GEngine->OnNetworkFailure().Remove(OnNetworkFailureDelegateHandle);
			OnNetworkFailureDelegateHandle.Reset();
		}
	}
}

#pragma endregion PrivateUtilityFunctions

#pragma region CallbackFunctions

void UPNetworkingInstanceSteam::OnCreateSessionComplete(FName NewName, bool bWasSuccessfull)
{
	if (CreateSessionCompleteDelegateHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionPointer()->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		CreateSessionCompleteDelegateHandle.Reset();
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnCreateSessionComplete: OnCreateSessionComplete delegate cleared!"));
	}

	if (!bWasSuccessfull)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnCreateSessionComplete: Creating Session error!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	if (!GEngine)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnCreateSessionComplete: GEngine is invalid!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	UWorld* World = GEngine->GetWorldContexts().Num() > 0 ? GEngine->GetWorldContexts()[0].World() : nullptr;
	if (!World)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnCreateSessionComplete: World not found!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	const FString ServerMap = MapPathToTravel + TEXT("?listen");
	const bool bServerTravelResult = World->ServerTravel(ServerMap);
	if (bServerTravelResult)
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnCreateSessionComplete: Server Travel Complete!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_VALID);
	}
	else
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnCreateSessionComplete: Server Travel Error!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
	}
}

void UPNetworkingInstanceSteam::OnInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, FUniqueNetIdPtr FriendID, const FOnlineSessionSearchResult& InviteResult)
{
	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionPointer();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnInviteAccepted: SessionInterface is invalid!"));
		return;
	}

	TempPrevSessionState = FPNetworkingModule::GetLocalSessionCurrentState();

	if (bWasSuccessful)
	{
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_PENDING);

		if (SessionInterface->GetNamedSession(FPNetworkingModule::GetSessionName()))
		{
			UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnInviteAccepted: Session existing, need to destroy it!"));

			LastInviteResult = InviteResult;
			OnClientNewInviteAcceptionDestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
				FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnClientNewInviteAcceptionDestroySessionComplete));

			if (FPNetworkingModule::GetOnlineSessionPointer()->DestroySession(FPNetworkingModule::GetSessionName()))
			{
				FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_DESTROYING);
				UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnInviteAccepted: DestroySession request true!"));
			}
			else
			{
				if (OnClientNewInviteAcceptionDestroySessionCompleteHandle.IsValid())
				{
					SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnClientNewInviteAcceptionDestroySessionCompleteHandle);
					OnClientNewInviteAcceptionDestroySessionCompleteHandle.Reset();
				}

				FPNetworkingModule::SetLocalSessionCurrentState(TempPrevSessionState);
			}

			return;
		}

		JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnJoinSessionComplete));
		const bool bHasJoined = SessionInterface->JoinSession(0, FPNetworkingModule::GetSessionName(), InviteResult);

		if (bHasJoined)
		{
			UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnInviteAccepted: Invite Acception Success by %s"), *InviteResult.Session.OwningUserName);
		}
		else
		{
			UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnInviteAccepted: Invite Acception Error!"));
			FPNetworkingModule::SetLocalSessionCurrentState(TempPrevSessionState);
		}
	}
	else
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("Invite Acception Error!"));
	}
}

void UPNetworkingInstanceSteam::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionPointer();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnJoinSessionComplete: SessionInterface is invalid!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	if (JoinSessionCompleteDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		JoinSessionCompleteDelegateHandle.Reset();
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnJoinSessionComplete: Join Session failed!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	FString ConnectInfo;
	if (!SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnJoinSessionComplete: Failed to get resolved connect string!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	UWorld* World = GEngine->GetWorldContexts().Num() > 0 ? GEngine->GetWorldContexts()[0].World() : nullptr;
	if (!World)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnJoinSessionComplete: World is null!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	if (!PlayerController)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnJoinSessionComplete: PlayerController is null!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_VALID);
	PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);

	UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnJoinSessionComplete: Client Travel to: %s"), *ConnectInfo);
}

void UPNetworkingInstanceSteam::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnNetworkFailure: Error -> %s"), *ErrorString);
	IOnlineSessionPtr OnlineSession = FPNetworkingModule::GetOnlineSessionPointer();
	if (!OnlineSession.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnNetworkFailure: OnlineSession is invalid!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	if (FPNetworkingModule::GetOnlineSessionPointer()->GetNamedSession(FPNetworkingModule::GetSessionName()))
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnNetworkFailure: Session existing, need to destroy it!"));
		OnClientDestroySessionCompleteHandle = FPNetworkingModule::GetOnlineSessionPointer()->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnClientDestroySessionComplete));

		if (FPNetworkingModule::GetOnlineSessionPointer()->DestroySession(FPNetworkingModule::GetSessionName()))
		{
			UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnNetworkFailure: DestroySession request true!"));
			FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_DESTROYING);
		}

		return;
	}

	FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
}

void UPNetworkingInstanceSteam::OnDestroySessionCompleteFromNewHostingUser(FName SessionName, bool bWasSuccessfull)
{
	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionPointer();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnDestroySessionCompleteFromNewHostingUser: SessionInterface is invalid!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	if (OnDestroySessionCompleteFromNewHostingUserHandle.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteFromNewHostingUserHandle);
		OnDestroySessionCompleteFromNewHostingUserHandle.Reset();
	}

	if (bWasSuccessfull)
	{
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnDestroySessionCompleteFromNewHostingUser: Session destroyed! -> %s"), *SessionName.ToString());
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_PENDING);
		CreateSession();
	}
	else
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnDestroySessionCompleteFromNewHostingUser: Session not destroyed!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
	}
}

void UPNetworkingInstanceSteam::OnClientDestroySessionComplete(FName SessionName, bool bWasSuccessfull)
{
	IOnlineSessionPtr OnlineSession = FPNetworkingModule::GetOnlineSessionPointer();
	if (!OnlineSession.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnClientDestroySessionComplete: OnlineSession is null!"));
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		return;
	}

	if (OnClientDestroySessionCompleteHandle.IsValid())
	{
		OnlineSession->ClearOnDestroySessionCompleteDelegate_Handle(OnClientDestroySessionCompleteHandle);
		OnClientDestroySessionCompleteHandle.Reset();
	}

	if (bWasSuccessfull)
	{
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnClientDestroySessionComplete: Session %s destroyed"), *SessionName.ToString());
	}
}

void UPNetworkingInstanceSteam::OnClientNewInviteAcceptionDestroySessionComplete(FName SessionName, bool bWasSuccessfull)
{
	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionPointer();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnClientNewInviteAcceptionDestroySessionComplete: SessionInterface is invalid!"));
		FPNetworkingModule::SetLocalSessionCurrentState(TempPrevSessionState);
		return;
	}

	if (OnClientNewInviteAcceptionDestroySessionCompleteHandle.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnClientNewInviteAcceptionDestroySessionCompleteHandle);
		OnClientNewInviteAcceptionDestroySessionCompleteHandle.Reset();
	}

	if (bWasSuccessfull)
	{
		FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_PENDING);

		JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnJoinSessionComplete));

		const bool bHasJoined = FPNetworkingModule::GetOnlineSessionPointer()->JoinSession(0, FPNetworkingModule::GetSessionName(), LastInviteResult);
		if (bHasJoined)
		{
			UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("OnClientNewInviteAcceptionDestroySessionComplete: Invite Acception Success by %s"), *LastInviteResult.Session.OwningUserName);
		}
		else
		{
			UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("OnClientNewInviteAcceptionDestroySessionComplete: Invite Acception Error!"));
			FPNetworkingModule::SetLocalSessionCurrentState(ELocalSessionState::SESSION_INVALID);
		}
	}
	else
	{
		FPNetworkingModule::SetLocalSessionCurrentState(TempPrevSessionState);
	}
}

#pragma endregion CallbackFunctions

#pragma region SteamworksFunctions

UTexture2D* UPNetworkingInstanceSteam::GetAvatar(const CSteamID SteamID, int32& QueryResult)
{
	int32 AvatarID = SteamFriends()->GetLargeFriendAvatar(SteamID);

	if (AvatarID == 0)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetAvatar: ERROR: Invalid Avatar ID!"));
		QueryResult = 0;
		return nullptr;
	}

	if (AvatarID == -1)
	{
		QueryResult = -1;
		return nullptr;
	}

	// Avatar size.
	uint32 Width, Height;
	if (!SteamUtils()->GetImageSize(AvatarID, &Width, &Height))
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetAvatar: ERROR: GetImageSize()"));
		return nullptr;
	}

	// Avatar RGBA datas.
	TArray<uint8> AvatarData;
	const uint32 BufferSize = Width * Height * 4;
	AvatarData.Reserve(BufferSize);
	if (!SteamUtils()->GetImageRGBA(AvatarID, AvatarData.GetData(), BufferSize))
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetAvatar: ERROR: GetImageRGBA()"));
		return nullptr;
	}

	// Apply RGBA datas into texture.
	UTexture2D* AvatarTexture = UTexture2D::CreateTransient(Width, Height, EPixelFormat::PF_R8G8B8A8);
	if (!AvatarTexture)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetAvatar: ERROR: Invalid Avatar Texture!"));
		return nullptr;
	}

	void* TextureData = AvatarTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, AvatarData.GetData(), BufferSize);
	AvatarTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

	AvatarTexture->UpdateResource();

	QueryResult = 1;
	return AvatarTexture;
}

int32 UPNetworkingInstanceSteam::GetLocalUserAvatarRecursive(TSharedPtr<FOnLocalAvatarReady> Callback)
{
	int32 QueryResult = 0;

	ISteamUser* SteamUserInterface = SteamUser();
	if(!SteamUserInterface)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetLocalUserAvatarRecursive: SteamUserInterface steamwork_sdk not valid!"));
		return 0;
	}

	const CSteamID SteamID = SteamUserInterface->GetSteamID();
	if (!SteamID.IsValid())
	{
		return 0;
	}

	UTexture2D* AvatarBuffer = GetAvatar(SteamID, QueryResult);

	if (AvatarBuffer != nullptr && QueryResult == 1)
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateLocalUser.Unbind();
		}
		else
		{
			return 0;
		}

		if (AvatarBuffer && Callback.IsValid())
		{
			Callback->ExecuteIfBound(AvatarBuffer);
		}
		else
		{
			return 0;
		}
	}
	else if (AvatarBuffer == nullptr && QueryResult == -1)
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateLocalUser.Unbind();
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateLocalUser.BindLambda([Callback](AvatarImageLoaded_t* pCallback)
				{
					if (pCallback)
					{
						AsyncTask(ENamedThreads::GameThread, [Callback]()
							{
								UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("GetLocalUserAvatarRecursive: Callback AvatarImageLoaded ready from SteamAPI!"));
								UPNetworkingInstanceSteam::GetUniqueInstance()->GetLocalUserAvatarRecursive(Callback);
							}
						);
					}
				}
			);
		}
		else
		{
			return 0;
		}

		return -1;
	}
	else
	{
		return 0;
	}

	return 1;
}

int32 UPNetworkingInstanceSteam::GetRequestedFriendAvatarRecursive(const CSteamID SteamID, TSharedPtr<FOnRequestedFriendAvatarReady> Callback)
{
	int32 QueryResult = 0;
	
	UTexture2D* RequestedFriendAvatar;
	RequestedFriendAvatar = GetAvatar(SteamID, QueryResult);

	if (RequestedFriendAvatar != nullptr && QueryResult == 1)
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyFriendRequested.Unbind();
		}
		else
		{
			return 0;
		}

		if (RequestedFriendAvatar && Callback.IsValid())
		{
			Callback->ExecuteIfBound(RequestedFriendAvatar);
		}
		else
		{
			return 0;
		}
	}
	else if (RequestedFriendAvatar == nullptr && QueryResult == -1)
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyFriendRequested.Unbind();
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyFriendRequested.BindLambda([SteamID, Callback](AvatarImageLoaded_t* pCallback)
				{
					if (pCallback)
					{
						AsyncTask(ENamedThreads::GameThread, [SteamID, Callback]()
							{
								UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("GetRequestedFriendAvatarRecursive: Callback AvatarImageLoaded ready from SteamAPI!"));
								UPNetworkingInstanceSteam::GetUniqueInstance()->GetRequestedFriendAvatarRecursive(SteamID, Callback);
							}
						);
					}
				}
			);
		}
		else
		{
			return 0;
		}

		return -1;
	}
	else
	{
		return 0;
	}

	return 1;
}

int32 UPNetworkingInstanceSteam::GetFriendsAvatarRecursive(TSharedPtr<FOnFriendsAvatarReady> Callback)
{
	bool bPendingFlag = false;
	int32 QueryResult = 0;

	ISteamFriends* SteamFriendsInterface = SteamFriends();
	if (!SteamFriendsInterface)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetFriendsAvatarRecursive: SteamUserInterface steamwork_sdk not valid!"));
		return 0;
	}

	int32 FriendsCount = SteamFriendsInterface->GetFriendCount(k_EFriendFlagImmediate);
	if (FriendsCount < 0)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetFriendsAvatarRecursive: ERROR: GetFriendsCount()"));
		FriendsCount = 0;
		return 0;
	}

	TArray<UTexture2D*> FriendsAvatar;
	for (int32 Index = 0; Index < FriendsCount; Index++)
	{
		const CSteamID CurrentSteamID = SteamFriendsInterface->GetFriendByIndex(Index, k_EFriendFlagImmediate);
		UTexture2D* AvatarBuffer = GetAvatar(CurrentSteamID, QueryResult);

		if (AvatarBuffer != nullptr && QueryResult == 1)
		{
			FriendsAvatar.Add(AvatarBuffer);
		}
		else if (AvatarBuffer == nullptr && QueryResult == -1)
		{
			bPendingFlag = true;
			break;
		}
		else
		{
			return 0;
		}
	}

	if (!bPendingFlag)
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateFriendList.Unbind();
		}
		else
		{
			return 0;
		}

		if (FriendsAvatar.Num() > 0 && Callback.IsValid())
		{
			Callback->ExecuteIfBound(FriendsAvatar);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateFriendList.Unbind();
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateFriendList.BindLambda([Callback](AvatarImageLoaded_t* pCallback)
				{
					if (pCallback)
					{
						AsyncTask(ENamedThreads::GameThread, [Callback]()
							{
								UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("GetFriendsAvatarRecursive: Callback AvatarImageLoaded ready from SteamAPI!"));
								UPNetworkingInstanceSteam::GetUniqueInstance()->GetFriendsAvatarRecursive(Callback);
							}
						);
					}
				}
			);
		}
		else
		{
			return 0;
		}

		return -1;
	}

	return 1;
}

int32 UPNetworkingInstanceSteam::GetPlayerDataRecursive(const bool bAlphabeticalSort, TSharedPtr<FOnFriendsDataReady> Callback)
{
	TArray<FUserSteamData> UserSteamData;
	bool bPendingFlag = false;
	int32 QueryResult = 0;

	ISteamFriends* SteamFriendsInterface = SteamFriends();
	if (!SteamFriendsInterface)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetPlayerDataRecursive: SteamUserInterface steamworks_sdk not valid!"));
		return 0;
	}

	int32 FriendsCount = SteamFriendsInterface->GetFriendCount(k_EFriendFlagImmediate);
	if (FriendsCount < 0)
	{
		UE_LOG(LogSteamNetworkingPlugin, Error, TEXT("GetPlayerDataRecursive: ERROR: GetFriendsCount()"));
		FriendsCount = 0;
		return 0;
	}

	// Just online friends (better: not offline).
	UserSteamData.Reserve(GetOnlineFriendsFromFriendCount(FriendsCount));

	for (int32 Index = 0; Index < FriendsCount; Index++)
	{
		const CSteamID CurrentSteamID = SteamFriendsInterface->GetFriendByIndex(Index, k_EFriendFlagImmediate);
		const EPersonaState CurrentPlayerState = SteamFriendsInterface->GetFriendPersonaState(CurrentSteamID);

		// Just online friends (better: not offline).
		if (CurrentPlayerState != EPersonaState::k_EPersonaStateOffline)
		{
			const FString CurrentNickname(SteamFriendsInterface->GetFriendPersonaName(CurrentSteamID));
			UTexture2D* CurrentTexture = GetAvatar(CurrentSteamID, QueryResult);

			if (CurrentTexture != nullptr && QueryResult == 1)
			{
				UserSteamData.Add(FUserSteamData(static_cast<int32>(CurrentSteamID.GetAccountID()), FText::FromString(CurrentNickname), CurrentTexture));
			}
			else if (CurrentTexture == nullptr && QueryResult == -1)
			{
				bPendingFlag = true;
				break;
			}
			else
			{
				return 0;
			}
		}
	}

	if (bAlphabeticalSort && UserSteamData.Num() > 0)
	{
		AlphabeticalSortFriends(UserSteamData);
	}

	if (!bPendingFlag)
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyFriendListData.Unbind();
		}
		else
		{
			return 0;
		}

		if (UserSteamData.Num() > 0 && Callback.IsValid())
		{
			Callback->ExecuteIfBound(UserSteamData);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyFriendListData.Unbind();
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyFriendListData.BindLambda([bAlphabeticalSort, Callback](AvatarImageLoaded_t* pCallback)
				{
					if (pCallback)
					{
						AsyncTask(ENamedThreads::GameThread, [bAlphabeticalSort, Callback]()
							{
								UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("GetPlayerDataRecursive: Callback AvatarImageLoaded ready from SteamAPI!"));
								//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Emerald, TEXT("RE-Query GetAvatar"));

								UPNetworkingInstanceSteam::GetUniqueInstance()->GetPlayerDataRecursive(bAlphabeticalSort, Callback);
							}
						);
					}
				}
			);
		}
		else
		{
			return 0;
		}

		return -1;
	}

	return 1; 
}

#pragma endregion SteamworksFunctions