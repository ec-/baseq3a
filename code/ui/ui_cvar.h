#ifdef EXTERN_UI_CVAR
	#define UI_CVAR( vmCvar, cvarName, defaultString, cvarFlags ) extern vmCvar_t vmCvar;
#endif

#ifdef DECLARE_UI_CVAR
	#define UI_CVAR( vmCvar, cvarName, defaultString, cvarFlags ) vmCvar_t vmCvar;
#endif

#ifdef UI_CVAR_LIST
	#define UI_CVAR( vmCvar, cvarName, defaultString, cvarFlags ) { & vmCvar, cvarName, defaultString, cvarFlags },
#endif

UI_CVAR( ui_ffa_fraglimit, "ui_ffa_fraglimit", "20", CVAR_ARCHIVE )
UI_CVAR( ui_ffa_timelimit, "ui_ffa_timelimit", "0", CVAR_ARCHIVE )

UI_CVAR( ui_tourney_fraglimit, "ui_tourney_fraglimit", "0", CVAR_ARCHIVE )
UI_CVAR( ui_tourney_timelimit, "ui_tourney_timelimit", "15", CVAR_ARCHIVE )

UI_CVAR( ui_team_fraglimit, "ui_team_fraglimit", "0", CVAR_ARCHIVE )
UI_CVAR( ui_team_timelimit, "ui_team_timelimit", "20", CVAR_ARCHIVE )
UI_CVAR( ui_team_friendly, "ui_team_friendly",  "1", CVAR_ARCHIVE )

UI_CVAR( ui_ctf_capturelimit, "ui_ctf_capturelimit", "8", CVAR_ARCHIVE )
UI_CVAR( ui_ctf_timelimit, "ui_ctf_timelimit", "30", CVAR_ARCHIVE )
UI_CVAR( ui_ctf_friendly, "ui_ctf_friendly",  "0", CVAR_ARCHIVE )

UI_CVAR( ui_arenasFile, "g_arenasFile", "", CVAR_INIT|CVAR_ROM )
UI_CVAR( ui_botsFile, "g_botsFile", "", CVAR_ARCHIVE|CVAR_LATCH )
UI_CVAR( ui_spScores1, "g_spScores1", "", CVAR_ARCHIVE )
UI_CVAR( ui_spScores2, "g_spScores2", "", CVAR_ARCHIVE )
UI_CVAR( ui_spScores3, "g_spScores3", "", CVAR_ARCHIVE )
UI_CVAR( ui_spScores4, "g_spScores4", "", CVAR_ARCHIVE )
UI_CVAR( ui_spScores5, "g_spScores5", "", CVAR_ARCHIVE )
UI_CVAR( ui_spAwards, "g_spAwards", "", CVAR_ARCHIVE )
UI_CVAR( ui_spVideos, "g_spVideos", "", CVAR_ARCHIVE )
UI_CVAR( ui_spSkill, "g_spSkill", "2", CVAR_ARCHIVE | CVAR_LATCH )

UI_CVAR( ui_spSelection, "ui_spSelection", "", CVAR_ROM )

UI_CVAR( ui_browserMaster, "ui_browserMaster", "0", CVAR_ARCHIVE )
UI_CVAR( ui_browserGameType, "ui_browserGameType", "0", CVAR_ARCHIVE )
UI_CVAR( ui_browserSortKey, "ui_browserSortKey", "4", CVAR_ARCHIVE )
UI_CVAR( ui_browserShowFull, "ui_browserShowFull", "1", CVAR_ARCHIVE )
UI_CVAR( ui_browserShowEmpty, "ui_browserShowEmpty", "1", CVAR_ARCHIVE )

UI_CVAR( ui_brassTime, "cg_brassTime", "2500", CVAR_ARCHIVE )
UI_CVAR( ui_drawCrosshair, "cg_drawCrosshair", "4", CVAR_ARCHIVE )
UI_CVAR( ui_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE )
UI_CVAR( ui_marks, "cg_marks", "1", CVAR_ARCHIVE )

UI_CVAR( ui_server1, "server1", "", CVAR_ARCHIVE )
UI_CVAR( ui_server2, "server2", "", CVAR_ARCHIVE )
UI_CVAR( ui_server3, "server3", "", CVAR_ARCHIVE )
UI_CVAR( ui_server4, "server4", "", CVAR_ARCHIVE )
UI_CVAR( ui_server5, "server5", "", CVAR_ARCHIVE )
UI_CVAR( ui_server6, "server6", "", CVAR_ARCHIVE )
UI_CVAR( ui_server7, "server7", "", CVAR_ARCHIVE )
UI_CVAR( ui_server8, "server8", "", CVAR_ARCHIVE )
UI_CVAR( ui_server9, "server9", "", CVAR_ARCHIVE )
UI_CVAR( ui_server10, "server10", "", CVAR_ARCHIVE )
UI_CVAR( ui_server11, "server11", "", CVAR_ARCHIVE )
UI_CVAR( ui_server12, "server12", "", CVAR_ARCHIVE )
UI_CVAR( ui_server13, "server13", "", CVAR_ARCHIVE )
UI_CVAR( ui_server14, "server14", "", CVAR_ARCHIVE )
UI_CVAR( ui_server15, "server15", "", CVAR_ARCHIVE )
UI_CVAR( ui_server16, "server16", "", CVAR_ARCHIVE )

UI_CVAR( ui_cdkeychecked, "ui_cdkeychecked", "0", CVAR_ROM )

#undef UI_CVAR
