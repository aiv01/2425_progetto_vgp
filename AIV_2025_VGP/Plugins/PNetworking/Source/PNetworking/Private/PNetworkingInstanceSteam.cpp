
// Copyright Epic Games, Inc. All Rights Reserved.

// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#include "PNetworkingInstanceSteam.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "UserSteamData.h"
#include "PNetworking.h"
#include "Kismet/GameplayStatics.h"
#include "Online.h"

UPNetworkingInstanceSteam* UPNetworkingInstanceSteam::NetInstanceSteamPtr = nullptr;

UPNetworkingInstanceSteam::UPNetworkingInstanceSteam()
{
}

UPNetworkingInstanceSteam::~UPNetworkingInstanceSteam()
{
}

UPNetworkingInstanceSteam* UPNetworkingInstanceSteam::GetUniqueInstance()
{
	if (NetInstanceSteamPtr == nullptr)
	{
		NetInstanceSteamPtr = NewObject<UPNetworkingInstanceSteam>();
		if (NetInstanceSteamPtr) 
		{
			NetInstanceSteamPtr->AddToRoot();
		}
	}

	return NetInstanceSteamPtr;
}

void UPNetworkingInstanceSteam::DeleteUniqueInstance()
{
	if (NetInstanceSteamPtr != nullptr)
	{
		NetInstanceSteamPtr->RemoveFromRoot();
		NetInstanceSteamPtr = nullptr;
	}
}

bool UPNetworkingInstanceSteam::GetAppID(FString& AppID)
{
	IOnlineSubsystem* OnlineSubsystemReference = FPNetworkingModule::GetOnlineSubsystemReference();

	if (!OnlineSubsystemReference)
	{
		return false;
	}

	AppID = OnlineSubsystemReference->GetAppId();

	return true;
}

bool UPNetworkingInstanceSteam::GetAccountName(FString& AccountName, const int32 UserID)
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

bool UPNetworkingInstanceSteam::GetFriendsList(const FOnFriendsListReady& Callback, const EFriendsLists::Type Query, const int32 LocalUserNum)
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

bool UPNetworkingInstanceSteam::GetOnlineFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum)
{
	return GetFriendsList(Callback, EFriendsLists::OnlinePlayers, LocalUserNum);
}

bool UPNetworkingInstanceSteam::GetAllFriendsList(const FOnFriendsListReady& Callback, const int32 LocalUserNum)
{
	return GetFriendsList(Callback, EFriendsLists::Default, LocalUserNum);
}

UTexture2D* UPNetworkingInstanceSteam::GetAvatar(const CSteamID SteamID, int32& QueryResult)
{
	int32 AvatarID = SteamFriends()->GetLargeFriendAvatar(SteamID);

	if (AvatarID == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Invalid Avatar ID!"));
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

	QueryResult = 1;
	return AvatarTexture;
}

int32 UPNetworkingInstanceSteam::GetLocalUserAvatar(const FOnLocalAvatarReady& Callback)
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return 0;
	}

	return GetLocalUserAvatarRecursive(MakeShared<FOnLocalAvatarReady>(Callback));
}

int32 UPNetworkingInstanceSteam::GetFriendsAvatar(const FOnFriendsAvatarReady& Callback)
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return 0;
	}

	return GetFriendsAvatarRecursive(MakeShared<FOnFriendsAvatarReady>(Callback));
}

int32 UPNetworkingInstanceSteam::GetPlayersData(const bool bAlphabeticalSort, const FOnFriendsDataReady& Callback)
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return 0;
	}

	return GetPlayerDataRecursive(bAlphabeticalSort, MakeShared<FOnFriendsDataReady>(Callback));
}

int32 UPNetworkingInstanceSteam::GetOnlineFriendsFromFriendCount(const int32 FriendsCount)
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

void UPNetworkingInstanceSteam::AlphabeticalSortFriends(TArray<FUserSteamData>& FriendsToSort)
{
	FriendsToSort.Sort([](const FUserSteamData& First, const FUserSteamData& Second) { return First.UserName.ToString() < Second.UserName.ToString(); });
}

#pragma region Debug
FString UPNetworkingInstanceSteam::GetUserNameFromSteamID(const int32 SteamID)
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return "";
	}

	CSteamID RealSteamID = ConvertInt32toCSteamID(SteamID);
	return FString(SteamFriends()->GetFriendPersonaName(RealSteamID));
}
void UPNetworkingInstanceSteam::OnDestroySessionComplete(FName sessionName, bool bWasSuccessfull)
{
	if (DestroySessionCompleteDelegateHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionReference()->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		DestroySessionCompleteDelegateHandle.Reset();

	}

	if (SessionParticipantLeftDelegateHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionReference()->ClearOnSessionParticipantLeftDelegate_Handle(SessionParticipantLeftDelegateHandle);
		SessionParticipantLeftDelegateHandle.Reset();
	}

	if (SessionParticipantRemovedDelegateHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionReference()->ClearOnSessionParticipantRemovedDelegate_Handle(SessionParticipantRemovedDelegateHandle);
		SessionParticipantRemovedDelegateHandle.Reset();
	}

	if (bWasSuccessfull)
	{
		UE_LOG(LogTemp, Error, TEXT("Session destroyed! -> %s"), *sessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Session not destroyed!"));
	}
}


void UPNetworkingInstanceSteam::DestroySession()
{
	DestroySessionCompleteDelegateHandle = FPNetworkingModule::GetOnlineSessionReference()->AddOnDestroySessionCompleteDelegate_Handle(
		FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnDestroySessionComplete));

	if (DestroySessionCompleteDelegateHandle.IsValid() && FPNetworkingModule::GetOnlineSessionReference()->DestroySession(FPNetworkingModule::GetSessionName()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Session client-side destroyed successfull!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Destroy Session"));
	}
}

int32 UPNetworkingInstanceSteam::GetFriendsAvatarRecursive(TSharedRef<FOnFriendsAvatarReady> Callback)
{
	bool bPendingFlag = false;
	int32 QueryResult = 0;

	int32 FriendsCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
	if (FriendsCount < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: GetFriendsCount()"));
		FriendsCount = 0;
	}

	TArray<UTexture2D*> FriendsAvatar;
	for (int32 Index = 0; Index < FriendsCount; Index++)
	{
		const CSteamID CurrentSteamID = SteamFriends()->GetFriendByIndex(Index, k_EFriendFlagImmediate);
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

		AsyncTask(ENamedThreads::GameThread, [FriendsAvatar, Callback]()
			{
				Callback->ExecuteIfBound(FriendsAvatar);
			}
		);
	}
	else
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateFriendList.Unbind();
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateFriendList.BindLambda([this, Callback](AvatarImageLoaded_t* pCallback)
				{
					if (pCallback)
					{
						AsyncTask(ENamedThreads::GameThread, [this, Callback]()
							{
								UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("Callback AvatarImageLoaded ready from SteamAPI!"));
								GetFriendsAvatarRecursive(Callback);
							}
						);
					}
				}
			);
		}

		return -1;
	}

	return 1;
}

int32 UPNetworkingInstanceSteam::GetPlayerDataRecursive(const bool bAlphabeticalSort, TSharedRef<FOnFriendsDataReady> Callback)
{
	TArray<FUserSteamData> UserSteamData;
	bool bPendingFlag = false;
	int32 QueryResult = 0;

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
			UTexture2D* CurrentTexture = GetAvatar(CurrentSteamID, QueryResult);

			if (CurrentTexture != nullptr && QueryResult == 1)
			{
				UserSteamData.Add(FUserSteamData(static_cast<int32>(CurrentSteamID.GetAccountID()),
					FText::FromString(CurrentNickname),
					CurrentTexture));
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

		AsyncTask(ENamedThreads::GameThread, [UserSteamData, Callback]()
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Emerald, TEXT("All Avatar ready!"));
				Callback->ExecuteIfBound(UserSteamData);
			}
		);
	}
	else
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyFriendListData.Unbind();
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyFriendListData.BindLambda([this, bAlphabeticalSort, Callback](AvatarImageLoaded_t* pCallback)
				{
					if (pCallback)
					{
						AsyncTask(ENamedThreads::GameThread, [this, bAlphabeticalSort, Callback]()
							{
								UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("Callback AvatarImageLoaded ready from SteamAPI!"));
								GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Emerald, TEXT("RE-Query GetAvatar"));

								GetPlayerDataRecursive(bAlphabeticalSort, Callback);
							}
						);
					}
				}
			);
		}

		return -1;
	}

	return 1;
}

int32 UPNetworkingInstanceSteam::GetLocalUserAvatarRecursive(TSharedRef<FOnLocalAvatarReady> Callback)
{
	int32 QueryResult = 0;
	const CSteamID SteamID = SteamUser()->GetSteamID();
	UTexture2D* AvatarBuffer = GetAvatar(SteamID, QueryResult);

	if (AvatarBuffer != nullptr && QueryResult == 1)
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateLocalUser.Unbind();
		}

		AsyncTask(ENamedThreads::GameThread, [AvatarBuffer, Callback]()
			{
				Callback->ExecuteIfBound(AvatarBuffer);
			}
		);
	}
	else if (AvatarBuffer == nullptr && QueryResult == -1)
	{
		if (FPNetworkingModule::GetSteamAPIManager().IsValid())
		{
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateLocalUser.Unbind();
			FPNetworkingModule::GetSteamAPIManager()->OnAvatarReadyDelegateLocalUser.BindLambda([this, Callback](AvatarImageLoaded_t* pCallback)
				{
					if (pCallback)
					{
						AsyncTask(ENamedThreads::GameThread, [this, Callback]()
							{
								UE_LOG(LogSteamNetworkingPlugin, Warning, TEXT("Callback AvatarImageLoaded ready from SteamAPI!"));
								GetLocalUserAvatarRecursive(Callback);
							}
						);
					}
				}
			);
		}

		return -1;
	}
	else
	{
		return 0;
	}

	return 1;
}

bool UPNetworkingInstanceSteam::ConvertCSteamIDToFUniqueNetID(const CSteamID SteamID, FUniqueNetIdPtr& CorrespondanceNetID)
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

CSteamID UPNetworkingInstanceSteam::ConvertInt32toCSteamID(const int32 SteamID)
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

void UPNetworkingInstanceSteam::OnInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, FUniqueNetIdPtr FriendID, const FOnlineSessionSearchResult& InviteResult)
{
	if (bWasSuccessful)
	{
		if (FPNetworkingModule::GetOnlineSessionReference()->IsPlayerInSession(
			FPNetworkingModule::GetSessionName(), *FPNetworkingModule::GetOnlineSubsystemReference()->GetIdentityInterface()->GetUniquePlayerId(0)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Sono già in una bellissima sessione!"));
		}

		JoinSessionCompleteDelegateHandle = FPNetworkingModule::GetOnlineSessionReference()->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnJoinSessionComplete));
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

void UPNetworkingInstanceSteam::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (JoinSessionCompleteDelegateHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionReference()->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		JoinSessionCompleteDelegateHandle.Reset();
	}

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

void UPNetworkingInstanceSteam::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
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

	if (FPNetworkingModule::GetOnlineSessionReference()->UnregisterPlayer(
		FPNetworkingModule::GetSessionName(), *FPNetworkingModule::GetOnlineSubsystemReference()->GetIdentityInterface()->GetUniquePlayerId(0)))
	{
		UE_LOG(LogTemp, Error, TEXT("SELF UNREGISTERED!"));
	}
	CheckAndDestroyAlreadyExistingSession();
	//FPNetworkingModule::GetOnlineSessionReference()->EndSession(FPNetworkingModule::GetSessionName());
	//FPNetworkingModule::GetOnlineSessionReference()->RemoveNamedSession(FPNetworkingModule::GetSessionName());


	// Local forse si riferisce a LAN.
	/*FPNetworkingModule::GetOnlineSessionReference()->UnregisterLocalPlayer(
		*FPNetworkingModule::GetOnlineSubsystemReference()->GetIdentityInterface()->GetUniquePlayerId(0), FPNetworkingModule::GetSessionName(), FOnUnregisterLocalPlayerCompleteDelegate::CreateStatic(&UPNetworkingInstanceSteam::OnLocalPlayerUnregistered));*/

	const FString MainMenuMap = TEXT("/Game/Custom/Networking/Maps/L_Gym_NetMainMenu");
	PlayerController->ClientTravel(MainMenuMap, ETravelType::TRAVEL_Absolute);
}

void UPNetworkingInstanceSteam::OnCreateSessionComplete(FName NewName, bool bWasSuccessfull)
{
	FPNetworkingModule::bIsComputingNewSession = false;

	if (CreateSessionCompleteDelegateHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionReference()->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		CreateSessionCompleteDelegateHandle.Reset();
		UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete cleared!"));
	}
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



bool UPNetworkingInstanceSteam::RequestSessionCreation(const int32 NumberPublicConnections,
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

	CheckAndDestroyAlreadyExistingSession();

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

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnCreateSessionComplete));

	OnSessionPlayerNetworkFailureHandle = SessionInterface->AddOnSessionFailureDelegate_Handle(FOnSessionFailureDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnSessionPlayerNetworkFailure));
	SessionParticipantLeftDelegateHandle = SessionInterface->AddOnSessionParticipantLeftDelegate_Handle(FOnSessionParticipantLeftDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnPlayerLeft));
	SessionParticipantRemovedDelegateHandle = SessionInterface->AddOnSessionParticipantRemovedDelegate_Handle(FOnSessionParticipantRemovedDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnPlayerRemoved));

	return SessionInterface->CreateSession(0, FPNetworkingModule::GetSessionName(), NewSessionSettings);
}

bool UPNetworkingInstanceSteam::InviteFriend(const int32 SteamID)
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

void UPNetworkingInstanceSteam::OnPlayerLeft(FName sessionName, const FUniqueNetId& uniqueIdPlayerLeft, EOnSessionParticipantLeftReason reason)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, TEXT("PLAYER LEFT"));
	UE_LOG(LogTemp, Warning, TEXT("Player %s left!"), *FPNetworkingModule::GetOnlineSubsystemReference()->GetIdentityInterface()->GetPlayerNickname(uniqueIdPlayerLeft));
	FPNetworkingModule::GetOnlineSessionReference()->UnregisterPlayer(sessionName, uniqueIdPlayerLeft);
}

void UPNetworkingInstanceSteam::OnPlayerRemoved(FName sessionName, const FUniqueNetId& uniqueIdPlayerLeft)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, TEXT("PLAYER REMOVED"));
	UE_LOG(LogTemp, Warning, TEXT("Player %s removed!"), *FPNetworkingModule::GetOnlineSubsystemReference()->GetIdentityInterface()->GetPlayerNickname(uniqueIdPlayerLeft));
	FPNetworkingModule::GetOnlineSessionReference()->UnregisterPlayer(sessionName, uniqueIdPlayerLeft);
}

void UPNetworkingInstanceSteam::OnLocalPlayerUnregistered(const FUniqueNetId& uniqueIdPlayerLeft, const bool bResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, TEXT("LOCAL PLAYER UNREGISTERED"));
	UE_LOG(LogTemp, Warning, TEXT("Player %s unregistered!"), *FPNetworkingModule::GetOnlineSubsystemReference()->GetIdentityInterface()->GetPlayerNickname(uniqueIdPlayerLeft));
}

void UPNetworkingInstanceSteam::OnSessionPlayerNetworkFailure(const FUniqueNetId& CrashedPlayerID, ESessionFailure::Type ErrorType)
{
	UE_LOG(LogTemp, Warning, TEXT("SESSION NET PROBLEMS DETECTED!"));
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, TEXT("PLAYER(S) CRASHED"));

	if (ErrorType == ESessionFailure::ServiceConnectionLost)
	{
		if (CrashedPlayerID.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Player %s removed!"), *FPNetworkingModule::GetOnlineSubsystemReference()->GetIdentityInterface()->GetPlayerNickname(CrashedPlayerID));
			FPNetworkingModule::GetOnlineSessionReference()->UnregisterPlayer(FPNetworkingModule::GetSessionName(), CrashedPlayerID);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Lobby named %s networking error!"), *FPNetworkingModule::GetSessionName().ToString());
		}
	}
}


void UPNetworkingInstanceSteam::CheckAndDestroyAlreadyExistingSession()
{
	IOnlineSessionPtr SessionInterface = FPNetworkingModule::GetOnlineSessionReference();
	if (!SessionInterface)
	{
		return;
	}

	FNamedOnlineSession* ExistingSession = FPNetworkingModule::GetOnlineSessionReference()->GetNamedSession(FPNetworkingModule::GetSessionName());
	if (ExistingSession)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sessione esistente!"));
		DestroySession();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sessione non trovata!"));
	}
}

bool UPNetworkingInstanceSteam::InitializeOnlineCallbacks()
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("AcceptInvite Callback Initialized!"));
	SessionUserInviteAcceptedDelegateHandle = FPNetworkingModule::GetOnlineSessionReference()->AddOnSessionUserInviteAcceptedDelegate_Handle(
		FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UPNetworkingInstanceSteam::OnInviteAccepted));

	if (GEngine)
	{
		OnNetworkFailureDelegateHandle = GEngine->OnNetworkFailure().AddUObject(this, &UPNetworkingInstanceSteam::OnNetworkFailure);
		UE_LOG(LogTemp, Warning, TEXT("Network failure delegate registered."));
	}

	return true;
}

bool UPNetworkingInstanceSteam::DeInitializeOnlineCallbacks()
{
	if (!FPNetworkingModule::IsOnlineAvailable())
	{
		return false;
	}

	if (SessionUserInviteAcceptedDelegateHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionReference()->ClearOnSessionUserInviteAcceptedDelegate_Handle(SessionUserInviteAcceptedDelegateHandle);
		SessionUserInviteAcceptedDelegateHandle.Reset();
	}

	if (SessionParticipantRemovedDelegateHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionReference()->ClearOnSessionParticipantRemovedDelegate_Handle(SessionParticipantRemovedDelegateHandle);
		SessionParticipantRemovedDelegateHandle.Reset();
	}

	if (SessionParticipantLeftDelegateHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionReference()->ClearOnSessionParticipantLeftDelegate_Handle(SessionParticipantLeftDelegateHandle);
		SessionParticipantLeftDelegateHandle.Reset();
	}

	if (OnUnregisterLocalPlayerDelegateHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionReference()->ClearOnUnregisterPlayersCompleteDelegate_Handle(OnUnregisterLocalPlayerDelegateHandle);
		OnUnregisterLocalPlayerDelegateHandle.Reset();
	}

	if (OnSessionPlayerNetworkFailureHandle.IsValid())
	{
		FPNetworkingModule::GetOnlineSessionReference()->ClearOnSessionFailureDelegate_Handle(OnSessionPlayerNetworkFailureHandle);
		OnSessionPlayerNetworkFailureHandle.Reset();
	}

	if (GEngine && OnNetworkFailureDelegateHandle.IsValid())
	{
		GEngine->OnNetworkFailure().Remove(OnNetworkFailureDelegateHandle);
		UE_LOG(LogTemp, Warning, TEXT("Network failure delegate registered."));
		OnNetworkFailureDelegateHandle.Reset();
	}

	return true;
}

