// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "g_local.h"


/*
=======================================================================

  SESSION DATA

Session data is the only data that stays persistant across level loads
and tournament restarts.
=======================================================================
*/

/*
================
G_WriteClientSessionData

Called on game shutdown
================
*/
void G_WriteClientSessionData( gclient_t *client ) {
	const char	*s;
	const char	*var;

	s = va("%i %i %i %i %i %i %i", 
		client->sess.sessionTeam,
		client->sess.spectatorTime,
		client->sess.spectatorState,
		client->sess.spectatorClient,
		client->sess.wins,
		client->sess.losses,
		client->sess.teamLeader
		);

	var = va( "session%i", client - level.clients );

	trap_Cvar_Set( var, s );
}


/*
================
G_ReadSessionData

Called on a reconnect
================
*/
void G_ReadClientSessionData( gclient_t *client ) {
	char	s[MAX_STRING_CHARS];
	const char	*var;
	int teamLeader;
	int spectatorState;
	int sessionTeam;

	var = va( "session%i", client - level.clients );
	trap_Cvar_VariableStringBuffer( var, s, sizeof(s) );

	Q_sscanf( s, "%i %i %i %i %i %i %i",
		&sessionTeam,
		&client->sess.spectatorTime,
		&spectatorState,
		&client->sess.spectatorClient,
		&client->sess.wins,
		&client->sess.losses,
		&teamLeader
		);

	client->sess.sessionTeam = (team_t)sessionTeam;
	client->sess.spectatorState = (spectatorState_t)spectatorState;
	client->sess.teamLeader = (qboolean)teamLeader;

	if ( (unsigned)client->sess.sessionTeam >= TEAM_NUM_TEAMS ) {
		client->sess.sessionTeam = TEAM_SPECTATOR;
	}
}


/*
================
G_ClearClientSessionData
================
*/
void G_ClearClientSessionData( gclient_t *client )
{
	const char	*var;

	var = va( "session%i", client - level.clients );
	trap_Cvar_Set( var, "" );
}


/*
================
G_InitSessionData

Called on a first-time connect
================
*/
void G_InitSessionData( gclient_t *client, const char *team, qboolean isBot ) {
	clientSession_t	*sess;
	
	sess = &client->sess;

	// initial team determination
	if ( g_gametype.integer >= GT_TEAM ) {
		if ( team[0] == 's' || team[0] == 'S' ) {
			// a willing spectator, not a waiting-in-line
			sess->sessionTeam = TEAM_SPECTATOR;
		} else {
			if ( g_autoJoin.integer & 2 ) {
				sess->sessionTeam = PickTeam( -1 );
			} else {
				// always spawn as spectator in team games
				if ( isBot == qfalse ) {
					sess->sessionTeam = TEAM_SPECTATOR;	
				} else  {
					// bind player to specified team
					if ( team[0] == 'r' || team[0] == 'R' ) {
						sess->sessionTeam = TEAM_RED;
					} else if ( team[0] == 'b' || team[0] == 'B' ) {
						sess->sessionTeam = TEAM_BLUE;
					} else {
						// or choose new
						sess->sessionTeam = PickTeam( -1 );
					}
				}
			}
		}
	} else {
		if ( team[0] == 's' || team[0] == 'S' ) {
			// a willing spectator, not a waiting-in-line
			sess->sessionTeam = TEAM_SPECTATOR;
		} else {
			switch ( g_gametype.integer ) {
			default:
			case GT_FFA:
			case GT_SINGLE_PLAYER:
				if ( g_maxGameClients.integer > 0 && level.numNonSpectatorClients >= g_maxGameClients.integer ) {
					sess->sessionTeam = TEAM_SPECTATOR;
				} else {
					if ( g_autoJoin.integer & 1 || isBot || g_gametype.integer == GT_SINGLE_PLAYER )
						sess->sessionTeam = TEAM_FREE;
					else
						sess->sessionTeam = TEAM_SPECTATOR;
				}
				break;
			case GT_TOURNAMENT:
				// if the game is full, go into a waiting mode
				if ( level.numNonSpectatorClients >= 2 ) {
					sess->sessionTeam = TEAM_SPECTATOR;
				} else {
					if ( g_autoJoin.integer & 1 || isBot )
						sess->sessionTeam = TEAM_FREE;
					else
						sess->sessionTeam = TEAM_SPECTATOR;
				}
				break;
			}
		}
	}

	sess->spectatorState = SPECTATOR_FREE;
	sess->spectatorTime = 0;
}


/*
==================
G_InitWorldSession

==================
*/
void G_InitWorldSession( void ) {
	char	s[MAX_STRING_CHARS];
	int			gt;

	trap_Cvar_VariableStringBuffer( "session", s, sizeof(s) );
	gt = atoi( s );
	
	// if the gametype changed since the last session, don't use any
	// client sessions
	if ( !*s || g_gametype.integer != gt ) {
		level.newSession = qtrue;
		G_Printf( "Gametype changed, clearing session data.\n" );
	}
}


/*
==================
G_WriteSessionData

==================
*/
void G_WriteSessionData( void ) {
	int		i;

	trap_Cvar_Set( "session", va("%i", g_gametype.integer) );

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_DISCONNECTED ) {
			G_WriteClientSessionData( &level.clients[i] );
		}
	}
}
