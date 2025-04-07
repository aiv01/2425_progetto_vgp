// Copyright Epic Games, Inc. All Rights Reserved.

// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#include "PNetworkingBPLibrary.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "UserSteamData.h"
#include "PNetworking.h"
#include "Kismet/GameplayStatics.h"

// To disable "strncpy" security warnings.
#pragma warning(push)
#pragma warning(disable:4996)
#include "AI/NavigationSystemBase.h"
#include "steam/steam_api.h"
#pragma warning(pop)

FDelegateHandle UPNetworkingBPLibrary::CreateSessionCompleteDelegateHandle;

UPNetworkingBPLibrary::UPNetworkingBPLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("BPL Constructor Called"));
}

bool UPNetworkingBPLibrary::GetAppID(FString& AppID)
{
	IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

	if (!OnlineSubsystemReference)
	{
		return false;
	}

	AppID = OnlineSubsystemReference->GetAppId();

	return true;
}

bool UPNetworkingBPLibrary::GetAccountName(FString& AccountName, const int32 UserID)
{
	IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

	if (!OnlineSubsystemReference)
	{
		return false;
	}

	IOnlineIdentityPtr OnlineIdentityPtr = OnlineSubsystemReference->GetIdentityInterface();
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

bool UPNetworkingBPLibrary::GetFriendsList(const FOnFriendsListReady& Callback, const EFriendsLists::Type Query, const int32 LocalUserNum)
{
	IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

	if (!OnlineSubsystemReference)
	{
		return false;
	}

	IOnlineFriendsPtr FriendsInterface = OnlineSubsystemReference->GetFriendsInterface();
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

			IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

			if (!OnlineSubsystemReference)
			{
				return;
			}

			IOnlineFriendsPtr OnlineFriendsPtr = OnlineSubsystemReference->GetFriendsInterface();
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

bool UPNetworkingBPLibrary::GetOnlineFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum)
{
	return GetFriendsList(Callback, EFriendsLists::OnlinePlayers, LocalUserNum);
}

bool UPNetworkingBPLibrary::GetAllFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum)
{
	return GetFriendsList(Callback, EFriendsLists::Default, LocalUserNum);
}

UTexture2D* UPNetworkingBPLibrary::GetAvatar(const CSteamID SteamID)
{
	int32 AvatarID = SteamFriends()->GetLargeFriendAvatar(SteamID);

	if (AvatarID < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Invalid Avatar ID!"));
		return nullptr;
	}

	// Avatar size.
	uint32 Width, Height;
	if (!SteamUtils()->GetImageSize(AvatarID, &Width, &Height))
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: GetImageSize()"));
		return nullptr;
	}

	// Avatar RGBA datas.
	TArray<uint8> AvatarData;
	const uint32 BufferSize = Width * Height * 4;
	AvatarData.Reserve(BufferSize);
	if (!SteamUtils()->GetImageRGBA(AvatarID, AvatarData.GetData(), BufferSize))
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: GetImageRGBA()"));
		return nullptr;
	}

	// Apply RGBA datas into texture.
	UTexture2D* AvatarTexture = UTexture2D::CreateTransient(Width, Height, EPixelFormat::PF_R8G8B8A8);
	if (!AvatarTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Invalid Avatar Texture!"));
		return nullptr;
	}

	void* TextureData = AvatarTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, AvatarData.GetData(), BufferSize);
	AvatarTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

	AvatarTexture->UpdateResource();

	return AvatarTexture;
}

UTexture2D* UPNetworkingBPLibrary::GetLocalUserAvatar()
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return nullptr;
	}

	const CSteamID SteamID = SteamUser()->GetSteamID(); // Local user.
	return GetAvatar(SteamID);
}

TArray<UTexture2D*> UPNetworkingBPLibrary::GetFriendsAvatar()
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return TArray<UTexture2D*>();
	}

	// Get friend's amount.
	int32 FriendsCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
	if (FriendsCount < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: GetFriendsCount()"));
		FriendsCount = 0;
	}

	TArray<UTexture2D*> FriendsAvatar; // Array of avatars.
	for (int32 Index = 0; Index < FriendsCount; Index++)
	{
		const CSteamID CurrentSteamID = SteamFriends()->GetFriendByIndex(Index, k_EFriendFlagImmediate);
		FriendsAvatar.Add(GetAvatar(CurrentSteamID));
	}

	return FriendsAvatar;
}

TArray<FUserSteamData> UPNetworkingBPLibrary::GetPlayersData(const bool bAlphabeticalSort)
{
	TArray<FUserSteamData> UserSteamData;

	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return UserSteamData;
	}

	int32 FriendsCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
	if (FriendsCount < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: GetFriendsCount()"));
		FriendsCount = 0;
	}

	// Just online friends (better: not offline).
	UserSteamData.Reserve(GetOnlineFriendsFromFriendCount(FriendsCount));

	for (int32 Index = 0; Index < FriendsCount; Index++)
	{
		const CSteamID CurrentSteamID = SteamFriends()->GetFriendByIndex(Index, k_EFriendFlagImmediate);
		const EPersonaState CurrentPlayerState = SteamFriends()->GetFriendPersonaState(CurrentSteamID);

		// Just online friends (better: not offline).
		if (CurrentPlayerState != EPersonaState::k_EPersonaStateOffline)
		{
			const FString CurrentNickname(SteamFriends()->GetFriendPersonaName(CurrentSteamID));
			UTexture2D* CurrentTexture = GetAvatar(CurrentSteamID);
			
			UserSteamData.Add(FUserSteamData(static_cast<int32>(CurrentSteamID.GetAccountID()), 
							  FText::FromString(CurrentNickname), 
							  CurrentTexture));
		}
	}

	if (bAlphabeticalSort)
	{
		AlphabeticalSortFriends(UserSteamData);
	}

	return UserSteamData;
}

int32 UPNetworkingBPLibrary::GetOnlineFriendsFromFriendCount(const int32 FriendsCount)
{
	int32 OnlineFriends = 0;

	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return OnlineFriends;
	}

	for (int32 Index = 0; Index < FriendsCount; Index++)
	{
		const CSteamID CurrentSteamID = SteamFriends()->GetFriendByIndex(Index, k_EFriendFlagImmediate);
		// UE_LOG(LogTemp, Warning, TEXT("ID: %u, NAME: %hs"), CurrentSteamID.GetAccountID(), SteamFriends()->GetFriendPersonaName(CurrentSteamID));
		const EPersonaState CurrentPlayerState = SteamFriends()->GetFriendPersonaState(CurrentSteamID);
		if (CurrentPlayerState != EPersonaState::k_EPersonaStateOffline)
		{
			OnlineFriends++;
		}
	}

	return OnlineFriends;
}

void UPNetworkingBPLibrary::AlphabeticalSortFriends(TArray<FUserSteamData>& FriendsToSort)
{
	FriendsToSort.Sort([](const FUserSteamData& First, const FUserSteamData& Second){ return First.UserName.ToString() < Second.UserName.ToString(); });
}

#pragma region Debug
FString UPNetworkingBPLibrary::GetUserNameFromSteamID(const int32 SteamID)
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return "";
	}

	CSteamID RealSteamID = ConvertInt32toCSteamID(SteamID);
	return FString(SteamFriends()->GetFriendPersonaName(RealSteamID));
}

void UPNetworkingBPLibrary::DestroySessionTest()
{
	auto lambda = FOnDestroySessionCompleteDelegate::CreateLambda(([](FName sessionName, bool bWasSuccessfull)
		{
			if (bWasSuccessfull)
			{
				UE_LOG(LogTemp, Error, TEXT("Session destroyed! -> %s"), *sessionName.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Session not destroyed!"));
			}
		}
	));

	if (FPNetworkingModule::GetOnlineSessionReference()->DestroySession(FPNetworkingModule::GetSessionName(), lambda))
	{
		UE_LOG(LogTemp, Warning, TEXT("Session client-side destroyed successfull!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Destroy Session "));
	}
	if (FPNetworkingModule::GetOnlineSessionReference()->UnregisterPlayer(FPNetworkingModule::GetSessionName(), *(FPNetworkingModule::GetOnlineSubsystemReference()->GetIdentityInterface()->GetUniquePlayerId(0))))
	{
		UE_LOG(LogTemp, Warning, TEXT("Self unregister to crashed session successfull!"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Client traveling back to Main Menu due to network failure."));
}

bool UPNetworkingBPLibrary::ConvertCSteamIDToFUniqueNetID(const CSteamID SteamID, FUniqueNetIdPtr& CorrespondanceNetID)
{
	IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

	if (!OnlineSubsystemReference)
	{
		return false;
	}

	uint64 SteamID64 = SteamID.ConvertToUint64();
	FString SteamIDString = FString::Printf(TEXT("%llu"), SteamID64);

	IOnlineIdentityPtr IdentityInterface = OnlineSubsystemReference->GetIdentityInterface();
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

CSteamID UPNetworkingBPLibrary::ConvertInt32toCSteamID(const int32 SteamID)
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return CSteamID();
	}

	const uint32 UnsignedSteamID = static_cast<uint32>(SteamID);
	CSteamID RealSteamID(UnsignedSteamID, EUniverse::k_EUniversePublic, EAccountType::k_EAccountTypeIndividual);
	return RealSteamID;
}
#pragma endregion Debug

void UPNetworkingBPLibrary::OnInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, FUniqueNetIdPtr FriendID, const FOnlineSessionSearchResult& InviteResult)
{
	if (bWasSuccessful)
	{
		FPNetworkingModule::GetOnlineSessionReference()->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateStatic(&UPNetworkingBPLibrary::OnJoinSessionComplete));
		const bool bHasJoined = FPNetworkingModule::GetOnlineSessionReference()->JoinSession(0, FPNetworkingModule::GetSessionName(), InviteResult);
		if (bHasJoined)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invite Acception Success by %s"), *InviteResult.Session.OwningUserName);

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invite Acception Error!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invite Acception Error!"));
	}
}

void UPNetworkingBPLibrary::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("Join Session failed!"));
		return;
	}

	FString ConnectInfo;
	if (!FPNetworkingModule::GetOnlineSessionReference()->GetResolvedConnectString(SessionName, ConnectInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get resolved connect string!"));
		return;
	}

	UWorld* World = GEngine->GetWorldContexts().Num() > 0 ? GEngine->GetWorldContexts()[0].World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null!"));
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is null!"));
		return;
	}

	PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
	UE_LOG(LogTemp, Warning, TEXT("Client Travel to: %s"), *ConnectInfo);
}

void UPNetworkingBPLibrary::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogTemp, Error, TEXT("Network Failure: %s"), *ErrorString);

	if (!GEngine)
	{
		UE_LOG(LogTemp, Error, TEXT("Engine error: GEngin invalid!"));
		return;
	}

	UWorld* CurrentWorld = GEngine->GetWorldContexts().Num() > 0 ? GEngine->GetWorldContexts()[0].World() : nullptr;
	if (!CurrentWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("OnNetworkFailure: World is null!"));
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(CurrentWorld, 0);
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("OnNetworkFailure: PlayerController is null!"));
		return;
	}

	const FString MainMenuMap = TEXT("/Game/Custom/Networking/Maps/L_Gym_Claudio");
	PlayerController->ClientTravel(MainMenuMap, ETravelType::TRAVEL_Absolute);

	 auto lambda = FOnDestroySessionCompleteDelegate::CreateLambda(([](FName sessionName, bool bWasSuccessfull)
		{
			if (bWasSuccessfull)
			{
				UE_LOG(LogTemp, Error, TEXT("Session destroyed! -> %s"), *sessionName.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Session not destroyed!"));
			}
		}
	));

	if (FPNetworkingModule::GetOnlineSessionReference()->DestroySession(FPNetworkingModule::GetSessionName(), lambda))
	{
		UE_LOG(LogTemp, Warning, TEXT("Session client-side destroyed successfull!"));
	}

	if (FPNetworkingModule::GetOnlineSessionReference()->UnregisterPlayer(FPNetworkingModule::GetSessionName(), *(FPNetworkingModule::GetOnlineSubsystemReference()->GetIdentityInterface()->GetUniquePlayerId(0))))
	{
		UE_LOG(LogTemp, Warning, TEXT("Self unregister to crashed session successfull!"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Client traveling back to Main Menu due to network failure."));
}

void UPNetworkingBPLibrary::OnCreateSessionComplete(FName NewName, bool bWasSuccessfull)
{
	FPNetworkingModule::bIsComputingNewSession = false;

	if (!bWasSuccessfull)
	{
		UE_LOG(LogTemp, Error, TEXT("Creating Session error!"));
		return;
	}

	UWorld* World = GEngine->GetWorldContexts().Num() > 0 ? GEngine->GetWorldContexts()[0].World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Server Travel Error!"));
		return;
	}

	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionReference();
	if (!SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	const bool bServerTravelResult = World->ServerTravel(TEXT("/Game/Custom/Networking/Maps/MapTest?listen")); // TODO: Initialize map path into blueprint
	if (bServerTravelResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server Travel Complete!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Server Travel Error!"));
	}
}

bool UPNetworkingBPLibrary::RequestSessionCreation(const FOnSessionCreationCompleted& Callback,
												   const int32 NumberPublicConnections, 
												   const int32 NumberPrivateConnections, 
												   const bool bIsLANMatch,
												   const bool bIsDedicated, 
												   const bool bShouldAdvertise,
												   const bool bUsesPresence,
												   const bool bAllowJoinViaPresenceFriendsOnly,
												   const bool bUseLobbiesIfAvailable)
{
	if (FPNetworkingModule::bIsComputingNewSession)
	{
		UE_LOG(LogTemp, Warning, TEXT("SteamError: Already computing a session!"));
		return false;
	}

	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionReference();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SteamError: Session interface not valid!"));
		return false;
	}

	FPNetworkingModule::bIsComputingNewSession = true;
	 
	FOnlineSessionSettings NewSessionSettings;
	NewSessionSettings.NumPublicConnections = NumberPublicConnections;
	NewSessionSettings.NumPrivateConnections = NumberPrivateConnections;
	NewSessionSettings.bIsLANMatch = bIsLANMatch;
	NewSessionSettings.bIsDedicated = bIsDedicated;
	NewSessionSettings.bShouldAdvertise = bShouldAdvertise;
	NewSessionSettings.bUsesPresence = bUsesPresence;
	NewSessionSettings.bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
	NewSessionSettings.bUseLobbiesIfAvailable = bUseLobbiesIfAvailable;
	NewSessionSettings.bAllowJoinInProgress = true;
	NewSessionSettings.bAllowJoinViaPresence = true;
	NewSessionSettings.bAllowInvites = true;

	// NewSessionSettings.Set(FPNetworkingModule::GetSessionSettingsKeyName(), NewSessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateStatic(&UPNetworkingBPLibrary::OnCreateSessionComplete));

	return SessionInterface->CreateSession(0, FPNetworkingModule::GetSessionName(), NewSessionSettings);
}

bool UPNetworkingBPLibrary::InviteFriend(const int32 SteamID)
{
	if (FPNetworkingModule::bIsComputingNewSession)
	{
		return false;
	}

	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionReference();
	if (!SessionInterface.IsValid())
	{
		return false;
	}

	const CSteamID FriendSteamID = ConvertInt32toCSteamID(SteamID);
	FUniqueNetIdPtr FriendUniqueNetID;
	
	if (ConvertCSteamIDToFUniqueNetID(FriendSteamID, FriendUniqueNetID))
	{
		FOnlineSession* OnlineSession = SessionInterface->GetNamedSession(FPNetworkingModule::GetSessionName());
		if (!OnlineSession)
		{
			UE_LOG(LogTemp, Warning, TEXT("Error: Session was not created!"));
			return false;
		}

		UE_LOG(LogTemp, Warning, TEXT("SessionID created: %s"), *OnlineSession->GetSessionIdStr());
		return SessionInterface->SendSessionInviteToFriend(0, FPNetworkingModule::GetSessionName(), *FriendUniqueNetID);
	}

	return false;
}

bool UPNetworkingBPLibrary::InitializeOnlineCallbacks()
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("AcceptInvite Callback Initialized!"));
	FPNetworkingModule::GetOnlineSessionReference()->AddOnSessionUserInviteAcceptedDelegate_Handle(FOnSessionUserInviteAcceptedDelegate::CreateStatic(&UPNetworkingBPLibrary::OnInviteAccepted));

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddStatic(&UPNetworkingBPLibrary::OnNetworkFailure);
		UE_LOG(LogTemp, Warning, TEXT("Network failure delegate registered."));
		return true;
	}

	auto playerLeft = FOnSessionParticipantLeftDelegate::CreateLambda(([](FName sessionName, const FUniqueNetId& uniqueIdPlayerLeft, EOnSessionParticipantLeftReason reason)
		{
			if (true)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player %s left!"), *FPNetworkingModule::GetOnlineSubsystemReference()->GetIdentityInterface()->GetPlayerNickname(uniqueIdPlayerLeft));
				FPNetworkingModule::GetOnlineSessionReference()->UnregisterPlayer(sessionName, uniqueIdPlayerLeft);
			}
		}
	));

	FPNetworkingModule::GetOnlineSessionReference()->AddOnSessionParticipantLeftDelegate_Handle(playerLeft);

	return true;
}
