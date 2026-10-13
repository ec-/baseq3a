#ifdef EXTERN_CG_CVAR
	#define CG_CVAR( vmCvar, cvarName, defaultString, cvarFlags, description ) extern vmCvar_t vmCvar;
#endif

#ifdef DECLARE_CG_CVAR
	#define CG_CVAR( vmCvar, cvarName, defaultString, cvarFlags, description ) vmCvar_t vmCvar;
#endif

#ifdef CG_CVAR_LIST
	#define CG_CVAR( vmCvar, cvarName, defaultString, cvarFlags, description ) { & vmCvar, cvarName, defaultString, cvarFlags, description },
#endif

CG_CVAR( cg_ignore, "cg_ignore", "0", 0, NULL ) // used for debugging
CG_CVAR( cg_autoswitch, "cg_autoswitch", "1", CVAR_ARCHIVE,
	"When picking up a weapon, automatically switch to it" )
CG_CVAR( cg_drawGun, "cg_drawGun", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_zoomFov, "cg_zoomfov", "22.5", CVAR_ARCHIVE,
	"Field Of View when zoomed, in degrees. Also see `cg_fov`" )
CG_CVAR( cg_fov, "cg_fov", "90", CVAR_ARCHIVE,
	"Field Of View, in degrees: the bigger it is the more \"zoomed out\" "
	"the view is, the more you see" )
CG_CVAR( cg_viewsize, "cg_viewsize", "100", CVAR_ARCHIVE,
	"How much of the screen space the game takes up" )
CG_CVAR( cg_shadows, "cg_shadows", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_gibs, "cg_gibs", "1", CVAR_ARCHIVE,
	"Make a bloody explosion when a player takes a lot of damage" )
CG_CVAR( cg_draw2D, "cg_draw2D", "1", CVAR_ARCHIVE,
	"Display HUD: health, ammo, score, etc" )
CG_CVAR( cg_drawStatus, "cg_drawStatus", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_drawTimer, "cg_drawTimer", "0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_drawFPS, "cg_drawFPS", "0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_drawSnapshot, "cg_drawSnapshot", "0", CVAR_ARCHIVE , NULL )
CG_CVAR( cg_draw3dIcons, "cg_draw3dIcons", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_drawIcons, "cg_drawIcons", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_drawAmmoWarning, "cg_drawAmmoWarning", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_drawAttacker, "cg_drawAttacker", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_drawSpeed, "cg_drawSpeed", "0", CVAR_ARCHIVE,
	"draw xy-speed\n"
	"0 - disabled\n"
	"1 - in top-right corner\n"
	"2 - under crosshair" )
CG_CVAR( cg_drawCrosshair, "cg_drawCrosshair", "4", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_drawRewards, "cg_drawRewards", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_drawWeaponSelect, "cg_drawWeaponSelect", "1", CVAR_ARCHIVE,
	"0 - disabled\n"
	"1 - horizontal with weapon name\n"
	"2 - horizontal with ammo counters\n"
	"3 - vectical with ammo counters\n"
	"use negative values to force permanent display" )
CG_CVAR( cg_crosshairSize, "cg_crosshairSize", "24", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_crosshairHealth, "cg_crosshairHealth", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_crosshairX, "cg_crosshairX", "0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_crosshairY, "cg_crosshairY", "0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_brassTime, "cg_brassTime", "2500", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_simpleItems, "cg_simpleItems", "0", CVAR_ARCHIVE,
	"Display in-game items as their 2D icons" )
CG_CVAR( cg_addMarks, "cg_marks", "1", CVAR_ARCHIVE,
	"Display bullet and gib impact marks on surfaces" )
CG_CVAR( cg_lagometer, "cg_lagometer", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_railTrailTime, "cg_railTrailTime", "400", CVAR_ARCHIVE , NULL )
CG_CVAR( cg_railTrailRadius, "cg_railTrailRadius", "0", CVAR_ARCHIVE ,
	"radius for linear light source, available only in q3e engine atm" )
CG_CVAR( cg_gun_frame, "cg_gun_frame", "", CVAR_ROM, NULL )
CG_CVAR( cg_gun_x, "cg_gunX", "0", CVAR_ARCHIVE,
	"Visually offset first-person weapon further/closer" )
CG_CVAR( cg_gun_y, "cg_gunY", "0", CVAR_ARCHIVE,
	"Visually offset first-person weapon left/right" )
CG_CVAR( cg_gun_z, "cg_gunZ", "0", CVAR_ARCHIVE,
	"Visually offset first-person weapon up/down" )
CG_CVAR( cg_centertime, "cg_centertime", "3", CVAR_CHEAT,
	"How long the messages printed in the center of the screen "
	"(such as \"You fragged Xaero\") stay for" )
CG_CVAR( cg_runpitch, "cg_runpitch", "0.002", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_runroll, "cg_runroll", "0.005", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_bobup , "cg_bobup", "0.005", CVAR_ARCHIVE,
	"How much to offset the view up when walking" )
CG_CVAR( cg_bobpitch, "cg_bobpitch", "0.002", CVAR_ARCHIVE,
	"How much to offset the look direction up/down when walking" )
CG_CVAR( cg_bobroll, "cg_bobroll", "0.002", CVAR_ARCHIVE,
	"How much to roll the camera clockwise/counterclockwise when walking" )
CG_CVAR( cg_swingSpeed, "cg_swingSpeed", "0.3", CVAR_CHEAT,
	"How fast the torso and legs rotate "
	"to match the look and movement direction" )
CG_CVAR( cg_animSpeed, "cg_animspeed", "1", CVAR_CHEAT, NULL )
CG_CVAR( cg_debugAnim, "cg_debuganim", "0", CVAR_CHEAT, NULL )
CG_CVAR( cg_debugPosition, "cg_debugposition", "0", CVAR_CHEAT, NULL )
CG_CVAR( cg_debugEvents, "cg_debugevents", "0", CVAR_CHEAT, NULL )
CG_CVAR( cg_errorDecay, "cg_errordecay", "100", 0, NULL )
CG_CVAR( cg_nopredict, "cg_nopredict", "0", 0, NULL )
CG_CVAR( cg_noPlayerAnims, "cg_noplayeranims", "0", CVAR_CHEAT, NULL )
CG_CVAR( cg_showmiss, "cg_showmiss", "0", 0, 
	"Print client-side prediction error messages. "
	"The bigger the number the more verbosity." )
CG_CVAR( cg_footsteps, "cg_footsteps", "1", CVAR_CHEAT, NULL )
CG_CVAR( cg_tracerChance, "cg_tracerchance", "0.4", CVAR_CHEAT, NULL )
CG_CVAR( cg_tracerWidth, "cg_tracerwidth", "1", CVAR_CHEAT, NULL )
CG_CVAR( cg_tracerLength, "cg_tracerlength", "100", CVAR_CHEAT, NULL )
CG_CVAR( cg_thirdPersonRange, "cg_thirdPersonRange", "40", CVAR_CHEAT, NULL )
CG_CVAR( cg_thirdPersonAngle, "cg_thirdPersonAngle", "0", CVAR_CHEAT, NULL )
CG_CVAR( cg_thirdPerson, "cg_thirdPerson", "0", 0, NULL )
CG_CVAR( cg_teamChatTime, "cg_teamChatTime", "3000", CVAR_ARCHIVE , NULL )
CG_CVAR( cg_teamChatHeight, "cg_teamChatHeight", "0", CVAR_ARCHIVE , NULL )
CG_CVAR( cg_forceModel, "cg_forceModel", "0", CVAR_ARCHIVE , NULL )
CG_CVAR( cg_predictItems, "cg_predictItems", "1", CVAR_ARCHIVE,
	"Client-side prediction of item pickups: "
	"when touching an item, don't wait for the server to confirm "
	"that we picked it up, instead assume that we did pick it up immediately, "
	"so that you can e.g. switch to that weapon with no delay" )
#ifdef MISSIONPACK
CG_CVAR( cg_deferPlayers, "cg_deferPlayers", "0", CVAR_ARCHIVE,
	"When a new player connects, don't load their model immediately "
	"to prevent lag. Instead, wait until you die or open the scoreboard" )
#else
CG_CVAR( cg_deferPlayers, "cg_deferPlayers", "1", CVAR_ARCHIVE,
	"When a new player connects, don't load their model immediately "
	"to prevent lag. Instead, wait until you die or open the scoreboard" )
#endif
CG_CVAR( cg_drawTeamOverlay, "cg_drawTeamOverlay", "0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_teamOverlayUserinfo, "teamoverlay", "0", CVAR_ROM | CVAR_USERINFO, NULL )
CG_CVAR( cg_stats, "cg_stats", "0", 0,
	"Print frame number in the console" )
CG_CVAR( cg_drawFriend, "cg_drawFriend", "1", CVAR_ARCHIVE,
	"Show a marker over teammates' heads" )
CG_CVAR( cg_teamChatsOnly, "cg_teamChatsOnly", "0", CVAR_ARCHIVE, NULL )
#ifdef MISSIONPACK
CG_CVAR( cg_noVoiceChats, "cg_noVoiceChats", "0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_noVoiceText, "cg_noVoiceText", "0", CVAR_ARCHIVE, NULL )
#endif
// the following variables are created in other parts of the system,
// but we also reference them here
CG_CVAR( cg_buildScript, "com_buildScript", "0", 0, NULL )	// force loading of all possible data amd error on failures
CG_CVAR( cg_paused, "cl_paused", "0", CVAR_ROM, NULL )
CG_CVAR( cg_blood, "com_blood", "1", CVAR_ARCHIVE, NULL )
#ifdef MISSIONPACK
CG_CVAR( cg_redTeamName, "g_redteam", DEFAULT_REDTEAM_NAME, CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO, NULL )
CG_CVAR( cg_blueTeamName, "g_blueteam", DEFAULT_BLUETEAM_NAME, CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO, NULL )
CG_CVAR( cg_currentSelectedPlayer, "cg_currentSelectedPlayer", "0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_currentSelectedPlayerName, "cg_currentSelectedPlayerName", "", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_singlePlayer, "ui_singlePlayerActive", "0", CVAR_USERINFO, NULL )
CG_CVAR( cg_enableDust, "g_enableDust", "0", CVAR_SERVERINFO, NULL )
CG_CVAR( cg_enableBreath, "g_enableBreath", "0", CVAR_SERVERINFO, NULL )
CG_CVAR( cg_singlePlayerActive, "ui_singlePlayerActive", "0", CVAR_USERINFO, NULL )
CG_CVAR( cg_recordSPDemo, "ui_recordSPDemo", "0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_recordSPDemoName, "ui_recordSPDemoName", "", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_obeliskRespawnDelay, "g_obeliskRespawnDelay", "10", CVAR_SERVERINFO, NULL )
CG_CVAR( cg_hudFiles, "cg_hudFiles", "ui/hud.txt", CVAR_ARCHIVE, NULL )
#endif
CG_CVAR( cg_cameraOrbit, "cg_cameraOrbit", "0", CVAR_CHEAT, NULL )
CG_CVAR( cg_cameraOrbitDelay, "cg_cameraOrbitDelay", "50", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_timescaleFadeEnd, "cg_timescaleFadeEnd", "1", 0, NULL )
CG_CVAR( cg_timescaleFadeSpeed, "cg_timescaleFadeSpeed", "0", 0, NULL )
CG_CVAR( cg_timescale, "timescale", "1", 0, NULL )
CG_CVAR( cg_scorePlum, "cg_scorePlums", "1", CVAR_USERINFO | CVAR_ARCHIVE, NULL )
CG_CVAR( cg_smoothClients, "cg_smoothClients", "0", CVAR_USERINFO | CVAR_ARCHIVE, NULL )
CG_CVAR( cg_cameraMode, "com_cameraMode", "0", CVAR_CHEAT, NULL )
CG_CVAR( cg_noTaunt, "cg_noTaunt", "0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_noProjectileTrail, "cg_noProjectileTrail", "0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_smallFont, "ui_smallFont", "0.25", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_bigFont, "ui_bigFont", "0.4", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_oldRail, "cg_oldRail", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_oldRocket, "cg_oldRocket", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_oldPlasma, "cg_oldPlasma", "1", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_trueLightning, "cg_trueLightning", "0.0", CVAR_ARCHIVE, NULL )
CG_CVAR( cg_hitSounds, "cg_hitSounds", "0", CVAR_ARCHIVE,
	"damage-based hitsounds\n"
	"\n"
	"0 - standard fixed tone\n"
	"1 - higher damage - lower tone\n"
	"1 - higher damage - higher tone\n"
	"\n"
	"requires proper from server in PERS_ATTACKEE_ARMOR\n" )
CG_CVAR( cg_enemyModel, "cg_enemyModel", "", CVAR_ARCHIVE, 
	"sets model for enemies\n"
	"\"pm\" means use current bright colored enemy model "
	"if model is known for mod i.e. there is possible to modify model colors\n"
	"if model is unknown (like \"alien\" or so) "
	"then it will be forced to \"sarge\"" )
CG_CVAR( cg_enemyColors, "cg_enemyColors", "", CVAR_ARCHIVE,
	"colorstring is a 3 [or 5] char-length string, where\n"
	"\n"
	"1st char - head color, all colors forced to \'???\' if not set\n"
	"2nd char - torso color, forced to white if not set\n"
	"3rd char - legs color, forced to white if not set\n"
	"\n"
	"4th char - optional, color1 override\n"
	"5th char - optional, color2 override\n"
	"\n"
	"'?' will be replaced to white in FFA games "
	"or corresponding team color (red or blue) in team games\n"
	"\n"
	"!!! will work only if cg_enemyModel is set !!!" )
CG_CVAR( cg_teamModel, "cg_teamModel", "", CVAR_ARCHIVE,
	"sets model for your teammates. See cg_enemyModel" )
CG_CVAR( cg_teamColors, "cg_teamColors", "", CVAR_ARCHIVE,
	"syntax and values is the same as for cg_enemyColors\n"
	"\n"
	"colorstring[4] and [5] will ALWAYS override color1 and color2 "
	"for spectated client\n"
	"\n"
	"but head, torso and legs colors will work corectly "
	"only if cg_teamModel is set" )
CG_CVAR( cg_deadBodyDarken, "cg_deadBodyDarken", "1", CVAR_ARCHIVE,
	"turn dead bodies into grey color\n"
	"\n"
	"works only if cg_enemyModel is set\n" )
CG_CVAR( cg_fovAdjust, "cg_fovAdjust", "0", CVAR_ARCHIVE,
	"Automatically rescale cg_fov from desired 4x3 screen ratio "
	"to widescreen resolution" )
CG_CVAR( cg_followKiller, "cg_followKiller", "0", CVAR_ARCHIVE, NULL )

#undef CG_CVAR
