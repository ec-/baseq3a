QA_SRC = \
 g_main $(QADIR)/g_syscalls.asm \
 bg_misc bg_lib bg_pmove bg_slidemove \
 q_math q_shared \
 ai_dmnet ai_dmq3 ai_team ai_main ai_chat ai_cmd ai_vcmd \
 g_active g_arenas g_bot g_client g_cmds g_combat g_items g_mem g_misc \
 g_missile g_mover g_rotation g_session g_spawn g_svcmds g_target g_team \
 g_trigger g_unlagged g_utils g_weapon \

CG_SRC = \
 cg_main $(CGDIR)/cg_syscalls.asm \
 cg_consolecmds cg_draw cg_drawtools cg_effects cg_ents cg_event cg_info \
 cg_localents cg_marks cg_players cg_playerstate cg_predict cg_scoreboard \
 cg_servercmds cg_snapshot cg_view cg_weapons \
 bg_slidemove bg_pmove bg_lib bg_misc \
 q_math q_shared \

UI_SRC = \
 ui_main $(UIDIR)/ui_syscalls.asm \
 ui_gameinfo ui_atoms ui_cinematics ui_connect ui_controls2 ui_demo2 \
 ui_mfield ui_credits ui_menu ui_ingame ui_confirm ui_setup ui_options \
 ui_display ui_sound ui_network ui_playermodel ui_players ui_playersettings \
 ui_preferences ui_qmenu ui_serverinfo ui_servers2 ui_sparena \
 ui_specifyserver ui_sppostgame ui_splevel ui_spskill ui_startserver ui_team \
 ui_video ui_addbots ui_removebots ui_teamorders ui_loadconfig ui_saveconfig \
 ui_cdkey ui_mods \
 bg_misc bg_lib \
 q_math q_shared \
