#ifdef EXTERN_G_CVAR
	#define G_CVAR( vmCvar, cvarName, defaultString, cvarFlags, trackChange, description ) extern vmCvar_t vmCvar;
	#define G_CVAR_EXT( vmCvar, cvarName, defaultString, cvarFlags, modificationCount, trackChange, teamShader, description ) extern vmCvar_t vmCvar;
#endif

#ifdef DECLARE_G_CVAR
	#define G_CVAR( vmCvar, cvarName, defaultString, cvarFlags, trackChange, description ) vmCvar_t vmCvar;
	#define G_CVAR_EXT( vmCvar, cvarName, defaultString, cvarFlags, modificationCount, trackChange, teamShader, description ) vmCvar_t vmCvar;
#endif

#ifdef G_CVAR_LIST
	#define G_CVAR( vmCvar, cvarName, defaultString, cvarFlags, trackChange, description ) { & vmCvar, cvarName, defaultString, cvarFlags, 0, trackChange, qfalse, description },
	#define G_CVAR_EXT( vmCvar, cvarName, defaultString, cvarFlags, modificationCount, trackChange, teamShader, description ) { & vmCvar, cvarName, defaultString, cvarFlags, modificationCount, trackChange, teamShader, description },
#endif

#define NO_FLAGS 0
// trackChange
#define TRACK qtrue
// Don't trackChange
#define NO_TRACK qfalse

// don't override the cheat state set by the system
G_CVAR( g_cheats, "sv_cheats", "", NO_FLAGS, NO_TRACK, NULL )

//G_CVAR( g_restarted, "g_restarted", "0", CVAR_ROM, NO_TRACK, NULL )
G_CVAR( g_mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM, NO_TRACK, NULL )
G_CVAR( sv_fps, "sv_fps", "30", CVAR_ARCHIVE, NO_TRACK, NULL )

// latched vars
G_CVAR( g_gametype, "g_gametype", "0", CVAR_SERVERINFO | CVAR_USERINFO | CVAR_LATCH, NO_TRACK, NULL )

G_CVAR( g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, NO_TRACK, NULL ) // allow this many total, including spectators
G_CVAR( g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, NO_TRACK, NULL ) // allow this many active

// change anytime vars
G_CVAR( g_dmflags, "dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, TRACK, NULL )
G_CVAR( g_fraglimit, "fraglimit", "20", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, TRACK, NULL )
G_CVAR( g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, TRACK, NULL )
G_CVAR( g_capturelimit, "capturelimit", "8", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, TRACK, NULL )

G_CVAR( g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, NO_TRACK, NULL )

G_CVAR( g_friendlyFire, "g_friendlyFire", "0", CVAR_ARCHIVE, TRACK, NULL )

G_CVAR( g_autoJoin, "g_autoJoin", "1", CVAR_ARCHIVE, NO_TRACK, NULL )
G_CVAR( g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE, NO_TRACK, NULL )

G_CVAR( g_warmup, "g_warmup", "20", CVAR_ARCHIVE, TRACK, NULL )
G_CVAR( g_log, "g_log", "games.log", CVAR_ARCHIVE, NO_TRACK, NULL )
G_CVAR( g_logSync, "g_logSync", "0", CVAR_ARCHIVE, NO_TRACK, NULL )

G_CVAR( g_password, "g_password", "", CVAR_USERINFO, NO_TRACK, NULL )

G_CVAR( g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, NO_TRACK, NULL )
G_CVAR( g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, NO_TRACK, NULL )

G_CVAR( g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, NO_TRACK, NULL )

G_CVAR( g_dedicated, "dedicated", "0", NO_FLAGS, NO_TRACK, NULL )

G_CVAR( g_speed, "g_speed", "320", NO_FLAGS, TRACK, NULL )
G_CVAR( g_gravity, "g_gravity", "800", NO_FLAGS, TRACK, NULL )
G_CVAR( g_knockback, "g_knockback", "1000", NO_FLAGS, TRACK, NULL )
G_CVAR( g_quadfactor, "g_quadfactor", "3", NO_FLAGS, TRACK, NULL )
G_CVAR( g_weaponRespawn, "g_weaponrespawn", "5", NO_FLAGS, TRACK, NULL )
G_CVAR( g_weaponTeamRespawn, "g_weaponTeamRespawn", "30", NO_FLAGS, TRACK, NULL )
G_CVAR( g_forcerespawn, "g_forcerespawn", "20", NO_FLAGS, TRACK, NULL )
G_CVAR( g_inactivity, "g_inactivity", "0", NO_FLAGS, TRACK, NULL )
G_CVAR( g_debugMove, "g_debugMove", "0", NO_FLAGS, NO_TRACK, NULL )
G_CVAR( g_debugDamage, "g_debugDamage", "0", NO_FLAGS, NO_TRACK, NULL )
G_CVAR( g_debugAlloc, "g_debugAlloc", "0", NO_FLAGS, NO_TRACK, NULL )
G_CVAR( g_motd, "g_motd", "", NO_FLAGS, NO_TRACK, NULL )
G_CVAR( g_blood, "com_blood", "1", NO_FLAGS, NO_TRACK, NULL )

G_CVAR( g_podiumDist, "g_podiumDist", "80", NO_FLAGS, NO_TRACK, NULL )
G_CVAR( g_podiumDrop, "g_podiumDrop", "70", NO_FLAGS, NO_TRACK, NULL )

G_CVAR( g_allowVote, "g_allowVote", "1", CVAR_ARCHIVE, NO_TRACK, NULL )
G_CVAR( g_listEntity, "g_listEntity", "0", NO_FLAGS, NO_TRACK, NULL )

G_CVAR( g_unlagged, "g_unlagged", "1", CVAR_SERVERINFO | CVAR_ARCHIVE, NO_TRACK, NULL )
G_CVAR( g_predictPVS, "g_predictPVS", "0", CVAR_ARCHIVE, NO_TRACK, NULL )

#ifdef MISSIONPACK
G_CVAR( g_obeliskHealth, "g_obeliskHealth", "2500", NO_FLAGS, NO_TRACK, NULL )
G_CVAR( g_obeliskRegenPeriod, "g_obeliskRegenPeriod", "1", NO_FLAGS, NO_TRACK, NULL )
G_CVAR( g_obeliskRegenAmount, "g_obeliskRegenAmount", "15", NO_FLAGS, NO_TRACK, NULL )
G_CVAR( g_obeliskRespawnDelay, "g_obeliskRespawnDelay", "10", CVAR_SERVERINFO, NO_TRACK, NULL )

G_CVAR( g_cubeTimeout, "g_cubeTimeout", "30", NO_FLAGS, NO_TRACK, NULL )
G_CVAR_EXT( g_redteam, "g_redteam", "Stroggs", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO, 0, TRACK, qtrue, NULL )
G_CVAR_EXT( g_blueteam, "g_blueteam", "Pagans", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO, 0, TRACK, qtrue, NULL )
G_CVAR( g_singlePlayer, "ui_singlePlayerActive", "", NO_FLAGS, NO_TRACK, NULL )

G_CVAR( g_enableDust, "g_enableDust", "0", CVAR_SERVERINFO, TRACK, NULL )
G_CVAR( g_enableBreath, "g_enableBreath", "0", CVAR_SERVERINFO, TRACK, NULL )
G_CVAR( g_proxMineTimeout, "g_proxMineTimeout", "20000", NO_FLAGS, NO_TRACK, NULL )
#endif
G_CVAR( g_smoothClients, "g_smoothClients", "1", NO_FLAGS, NO_TRACK, NULL )
G_CVAR( pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, NO_TRACK, NULL )
G_CVAR( pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, NO_TRACK, NULL )

G_CVAR( g_rotation, "g_rotation", "0", CVAR_ARCHIVE, NO_TRACK, NULL )

#undef G_CVAR
#undef G_CVAR_EXT
#undef NO_FLAGS
#undef TRACK
#undef NO_TRACK
