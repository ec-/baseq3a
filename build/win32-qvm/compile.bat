@echo off

rem make sure we have a safe environement
set LIBRARY=
set INCLUDE=

set cgamedir=..\..\..\..\code\cgame
set gamedir=..\..\..\..\code\game
set uidir=..\..\..\..\code\q3_ui

set tooldir=%~dp0tools\
set pk3=%~dp0pak8a.pk3

set cc1=%tooldir%q3lcc -DQ3_VM -DCGAME  -S -Wf-g -I%cgamedir% -I%gamedir% %1
set cc2=%tooldir%q3lcc -DQ3_VM -DQAGAME -S -Wf-g -I%gamedir% %1
set cc3=%tooldir%q3lcc -DQ3_VM -DQ3UI   -S -Wf-g -I%uidir% -I%gamedir% %1

rem its important to set -vq3 flag for new q3asm 
rem or qvm's will not run on original 1.32c binaries
set as1=%tooldir%q3asm -vq3 -r -m -v -o cgame -f %~dp0\cgame
set as2=%tooldir%q3asm -vq3 -r -m -v -o qagame -f %~dp0\game
set as3=%tooldir%q3asm -vq3 -r -m -v -o ui -f %~dp0\q3_ui

@rem ---------
@rem * CGAME *
@rem ---------

if exist vm\cgame goto cd1
mkdir vm\cgame
:cd1
cd vm\cgame
@if errorlevel 1 goto quit

%cc1% %cgamedir%\cg_main.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_consolecmds.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_draw.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_drawtools.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_effects.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_ents.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_event.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_info.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_localents.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_marks.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_players.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_playerstate.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_predict.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_scoreboard.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_servercmds.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_snapshot.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_view.c
@if errorlevel 1 goto quit
%cc1% %cgamedir%\cg_weapons.c

@if errorlevel 1 goto quit
%cc1% %gamedir%\bg_lib.c
@if errorlevel 1 goto quit
%cc1% %gamedir%\bg_misc.c
@if errorlevel 1 goto quit
%cc1% %gamedir%\bg_pmove.c
@if errorlevel 1 goto quit
%cc1% %gamedir%\bg_slidemove.c
@if errorlevel 1 goto quit
%cc1% %gamedir%\q_math.c
@if errorlevel 1 goto quit
%cc1% %gamedir%\q_shared.c
@if errorlevel 1 goto quit

@rem run assembler
%as1%
@if errorlevel 1 goto quit

move cgame.qvm ..
cd ..\..


@rem --------
@rem * GAME *
@rem --------

if exist vm\game goto cd2
mkdir vm\game
:cd2
cd vm\game
@if errorlevel 1 goto quit

%cc2% %gamedir%\g_main.c 
@if errorlevel 1 goto quit
%cc2% %gamedir%\ai_chat.c 
@if errorlevel 1 goto quit
%cc2% %gamedir%\ai_cmd.c 
@if errorlevel 1 goto quit
%cc2% %gamedir%\ai_dmnet.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\ai_dmq3.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\ai_main.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\ai_team.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\ai_vcmd.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\bg_lib.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\bg_misc.c 
@if errorlevel 1 goto quit
%cc2% %gamedir%\bg_pmove.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\bg_slidemove.c 
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_active.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_arenas.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_bot.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_client.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_cmds.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_combat.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_items.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_mem.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_misc.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_missile.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_mover.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_rotation.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_session.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_spawn.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_svcmds.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_target.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_team.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_trigger.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_unlagged.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_utils.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\g_weapon.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\q_math.c
@if errorlevel 1 goto quit
%cc2% %gamedir%\q_shared.c
@if errorlevel 1 goto quit

@rem run assembler
%as2%
@if errorlevel 1 goto quit

move qagame.qvm ..
cd ..\..


@rem --------
@rem ** UI **
@rem --------

if exist vm\ui goto cd3
mkdir vm\ui
:cd3
cd vm\ui
@if errorlevel 1 goto quit

%cc3% %uidir%\ui_addbots.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_atoms.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_cdkey.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_cinematics.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_confirm.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_connect.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_controls2.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_credits.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_demo2.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_display.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_gameinfo.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_ingame.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_loadconfig.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_main.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_menu.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_mfield.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_mods.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_network.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_options.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_playermodel.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_players.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_playersettings.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_preferences.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_qmenu.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_removebots.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_saveconfig.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_serverinfo.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_servers2.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_setup.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_sound.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_sparena.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_specifyserver.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_splevel.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_sppostgame.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_spskill.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_startserver.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_team.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_teamorders.c
@if errorlevel 1 goto quit
%cc3% %uidir%\ui_video.c  
@if errorlevel 1 goto quit

%cc3% %gamedir%\bg_lib.c 
@if errorlevel 1 goto quit
%cc3% %gamedir%\bg_misc.c
@if errorlevel 1 goto quit
%cc3% %gamedir%\q_math.c
@if errorlevel 1 goto quit
%cc3% %gamedir%\q_shared.c
@if errorlevel 1 goto quit

@rem run assembler
%as3%
@if errorlevel 1 goto quit

move ui.qvm ..
cd ..\..

copy vm\ui\ui.jts vm
copy vm\game\qagame.jts vm
copy vm\cgame\cgame.jts vm

%tooldir%7za.exe a -tzip -mx=9 -mpass=8 -mfb=255 -- %pk3% vm\*.*
rem rmdir /S /Q vm

cd ..\..\assets
@if errorlevel 1 goto quit
%tooldir%7za.exe a -tzip -mx=9 -mpass=8 -mfb=255 -r -- %pk3% *.*

:quit
pause
