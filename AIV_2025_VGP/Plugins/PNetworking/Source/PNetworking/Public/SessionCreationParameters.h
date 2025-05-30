// • Manuel Solano
// • Alessandro Caccamo
// • Claudio Dallai

#pragma once

#include "CoreMinimal.h"
#include "SessionCreationParameters.generated.h"

// Struct to get session parameters retrieved from an existing session.
USTRUCT(BlueprintType)
struct FGetSessionParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "The number of publicly available connections advertised."))
	int32 NumPublicConnections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "The number of connections that are private (invite/password) only."))
	int32 NumPrivateConnections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether this match is publicly advertised on the online service."))
	bool bShouldAdvertise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether joining in progress is allowed or not."))
	bool bAllowJoinInProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "This game will be lan only and not be visible to external players."))
	bool bIsLANMatch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether the server is dedicated or player hosted."))
	bool bIsDedicated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether the match allows invitations for this session or not."))
	bool bAllowInvites;

	FGetSessionParameters() : NumPublicConnections(0), NumPrivateConnections(0), bShouldAdvertise(false), bAllowJoinInProgress(false), bIsLANMatch(false), bIsDedicated(false), bAllowInvites(false) {}
	
};

// Struct to update session parameters from an existing session.
USTRUCT(BlueprintType)
struct FUpdateSessionParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "The number of publicly available connections advertised."))
	int32 NumPublicConnections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "The number of connections that are private (invite/password) only."))
	int32 NumPrivateConnections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether this match is publicly advertised on the online service."))
	bool bShouldAdvertise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether joining in progress is allowed or not."))
	bool bAllowJoinInProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "This game will be lan only and not be visible to external players."))
	bool bIsLANMatch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether the server is dedicated or player hosted."))
	bool bIsDedicated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether the match allows invitations for this session or not."))
	bool bAllowInvites;

	FUpdateSessionParameters() : NumPublicConnections(0), NumPrivateConnections(0), bShouldAdvertise(false), bAllowJoinInProgress(false), bIsLANMatch(false), bIsDedicated(false), bAllowInvites(false) {}
	
};

USTRUCT(BlueprintType)
struct FSessionCreationParameters
{
	GENERATED_BODY()

public:

	// Map Path to travel to after session creation (Path is: /Game/...).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Map Path to travel to after session creation (Path is: /Game/...)."))
	FString TravelToMapPath;

	// The number of publicly available connections advertised.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "The number of publicly available connections advertised."))
	int32 NumPublicConnections;

	// The number of connections that are private (invite/password) only.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "The number of connections that are private (invite/password) only."))
	int32 NumPrivateConnections;

	// Whether this match is publicly advertised on the online service.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether this match is publicly advertised on the online service."))
	bool bShouldAdvertise;

	// Whether joining in progress is allowed or not.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether joining in progress is allowed or not."))
	bool bAllowJoinInProgress;

	// This game will be lan only and not be visible to external players.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "This game will be lan only and not be visible to external players."))
	bool bIsLANMatch;

	// Whether the server is dedicated or player hosted.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether the server is dedicated or player hosted."))
	bool bIsDedicated;

	// Whether the match allows invitations for this session or not.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether the match allows invitations for this session or not."))
	bool bAllowInvites;

	// Whether to display user presence information or not.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether to display user presence information or not."))
	bool bUsesPresence;

	// Whether joining via player presence is allowed or not.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether joining via player presence is allowed or not."))
	bool bAllowJoinViaPresence;

	// Whether joining via player presence is allowed for friends only or not.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether joining via player presence is allowed for friends only or not."))
	bool bAllowJoinViaPresenceFriendsOnly;

	// Whether to prefer lobbies APIs if the platform supports them. Necessary if using Steam backend.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether to prefer lobbies APIs if the platform supports them. Necessary if using Steam backend."))
	bool bUseLobbiesIfAvailable;

	// Whether to create (and auto join) a voice chat room for the lobby, if the platform supports it. Currently not supported.
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SessionParameters", meta = (ToolTip = "Whether to create (and auto join) a voice chat room for the lobby, if the platform supports it."))
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
