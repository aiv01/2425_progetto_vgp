// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

#pragma once

#include "CoreMinimal.h"
#include "SessionCreationParameters.generated.h"

USTRUCT(BlueprintType)
struct FSessionCreationParameters
{
	GENERATED_BODY()

public:
	// The number of publicly available connections advertised.
	int32 NumPublicConnections;

	// The number of connections that are private (invite/password) only.
	int32 NumPrivateConnections;

	// Whether this match is publicly advertised on the online service.
	bool bShouldAdvertise;

	// Whether joining in progress is allowed or not.
	bool bAllowJoinInProgress;

	// This game will be lan only and not be visible to external players.
	bool bIsLANMatch;

	// Whether the server is dedicated or player hosted.
	bool bIsDedicated;

	// Whether the match allows invitations for this session or not.
	bool bAllowInvites;

	// Whether to display user presence information or not.
	bool bUsesPresence;

	// Whether joining via player presence is allowed or not.
	bool bAllowJoinViaPresence;

	// Whether joining via player presence is allowed for friends only or not.
	bool bAllowJoinViaPresenceFriendsOnly;

	// Whether to prefer lobbies APIs if the platform supports them. Necessary if using Steam backend.
	bool bUseLobbiesIfAvailable;

	// Whether to create (and auto join) a voice chat room for the lobby, if the platform supports it. Currently not supported.
	// bool bUseLobbiesVoiceChatIfAvailable;


	// Default constructor.
	FSessionCreationParameters()
		: NumPublicConnections(0)
		, NumPrivateConnections(0)
		, bShouldAdvertise(false)
		, bAllowJoinInProgress(false)
		, bIsLANMatch(false)
		, bIsDedicated(false)
		, bAllowInvites(false)
		, bUsesPresence(false)
		, bAllowJoinViaPresence(false)
		, bAllowJoinViaPresenceFriendsOnly(false)
		, bUseLobbiesIfAvailable(true)
		//, bUseLobbiesVoiceChatIfAvailable(false) 
	{}

	// Parameter constructor.
	FSessionCreationParameters
		(
			  int32 NumPublicConnectionsIn
			, int32 NumPrivateConnectionsIn
			, bool bShouldAdvertiseIn
			, bool bAllowJoinInProgressIn
			, bool bIsLANMatchIn
			, bool bIsDedicatedIn
			, bool bAllowInvitesIn
			, bool bUsesPresenceIn
			, bool bAllowJoinViaPresenceIn
			, bool bAllowJoinViaPresenceFriendsOnlyIn
			, bool bUseLobbiesIfAvailableIn
			//, bool bUseLobbiesVoiceChatIfAvailableIn
		)
			: NumPublicConnections(NumPublicConnectionsIn)
			, NumPrivateConnections(NumPrivateConnectionsIn)
			, bShouldAdvertise(bShouldAdvertiseIn)
			, bAllowJoinInProgress(bAllowJoinInProgressIn)
			, bIsLANMatch(bIsLANMatchIn)
			, bIsDedicated(bIsDedicatedIn)
			, bAllowInvites(bAllowInvitesIn)
			, bUsesPresence(bUsesPresenceIn)
			, bAllowJoinViaPresence(bAllowJoinViaPresenceIn)
			, bAllowJoinViaPresenceFriendsOnly(bAllowJoinViaPresenceFriendsOnlyIn)
			, bUseLobbiesIfAvailable(bUseLobbiesIfAvailableIn)
			//, bUseLobbiesVoiceChatIfAvailable(bUseLobbiesVoiceChatIfAvailableIn) 
	{}
};
